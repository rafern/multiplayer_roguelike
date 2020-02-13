#include "server_message.hpp"

std::unique_ptr<ClientMessage> ServerMessage::to_client() {
    return nullptr;
}

std::unique_ptr<ServerMessage> ServerMessage::from_buffer(Buffer& buffer, std::shared_ptr<Player> sender) {
    // Abort if header not received yet (type and data size)
    if(buffer.size() < 10)
        return nullptr;
    
    // Parse data size field
    uint64_t data_size;
    if(!buffer.get(data_size, 2))
        return nullptr;
    
    // Abort if body (data) not received
    if(buffer.size() < data_size + 10)
        return nullptr;
    
    // Parse type field
    uint16_t type;
    if(!buffer.get(type))
        return nullptr;
    
    // Clear header from buffer, full message received
    buffer.erase(10);
    
    // Create ServerMessage
    switch(type) {
        case GameMessageType::DoJoin:
            {
                // Body is a player name for Join messages
                std::string name;
                buffer.pop(name, data_size);
                return std::unique_ptr<ServerMessage>(new ServerMessageDoJoin(sender, name));
            }
            break;
        case GameMessageType::DoQuit:
            {
                // Body should be empty for DoQuit messages. Clear it if not
                if(data_size > 0)
                    buffer.erase(data_size);
                
                return std::unique_ptr<ServerMessage>(new ServerMessageDoQuit(sender));
            }
            break;
        case GameMessageType::DoChat:
            {
                // Body is a message name for DoChat messages
                std::string message;
                buffer.pop(message, data_size);
                return std::unique_ptr<ServerMessage>(new ServerMessageDoChat(sender, message));
            }
            break;
    }
    
    // Unknown message type or non-action message, clear body
    buffer.erase(data_size);
    return nullptr;
}

std::unique_ptr<ClientMessage> ServerMessageDoJoin::to_client() {
    return std::unique_ptr<ClientMessage>(
        new ClientMessageJoin(name)
    );
}

std::unique_ptr<ClientMessage> ServerMessageDoQuit::to_client() {
    return std::unique_ptr<ClientMessage>(
        new ClientMessageQuit(sender->name)
    );
}

std::unique_ptr<ClientMessage> ServerMessageDoChat::to_client() {
    return std::unique_ptr<ClientMessage>(
        new ClientMessageChat(sender->name, message)
    );
}