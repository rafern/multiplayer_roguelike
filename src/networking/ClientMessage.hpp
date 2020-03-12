#ifndef ROGUELIKE_CLIENT_MESSAGE_HPP_INCLUDED
#define ROGUELIKE_CLIENT_MESSAGE_HPP_INCLUDED
#include "../server/Player.hpp"
#include "../server/Map.h"
#include "Buffer.hpp"
#include <memory>

enum GameMessageType {
    Join = 0,
    Quit = 1,
    Chat = 2,
    MapTileData = 3,
    MapObjectData = 4,
    PlayerData = 5,
    DoJoin = 100,
    DoQuit = 101,
    DoChat = 102
};

/// A message sent to a client or by a client
class ClientMessage {
protected:
    /// Protected constructor
    ClientMessage(GameMessageType type, std::string senderName) :
        type(type),
        senderName(senderName)
    {}
    
    /// Helper for toBytes that automatically creates message from body
    const std::vector<uint8_t> toBytesHelper(const std::vector<uint8_t>& data) const;
    
public:
    /// Virtual destructor. Must be implemented if base classes do memory
    /// management
    virtual ~ClientMessage() = default;
    
    /// The message type. See GameMessageType
    const GameMessageType type;
    
    /// The name of the player that sent this message. If blank, the player
    /// hasn't joined yet or the message is an action to be sent to the server
    const std::string senderName;
    
    /// Converts client message to bytes (for networking). Has no body by
    /// default. Should be implemented, but not required
    virtual const std::vector<uint8_t> toBytes() const;
    
    /// Create a client message from a buffer. If there is enough data for a
    /// full message, buffer is (partially) popped and a new ClientMessage is
    /// returned, else, nullptr is returned and buffer is not popped. This is
    /// a factory
    static std::unique_ptr<ClientMessage> fromBuffer(Buffer& buffer);
};

struct ClientMessageJoin : public ClientMessage {
    /// Sent by the server if a client joined the game with a certain player
    /// name
    ClientMessageJoin(std::string senderName) :
        ClientMessage(GameMessageType::Join, senderName)
    {};
    
    ~ClientMessageJoin() = default;
    const std::vector<uint8_t> toBytes() const override;
};

struct ClientMessageQuit : public ClientMessage {
    /// Sent by the server if a client quit the game
    ClientMessageQuit(std::string senderName) :
        ClientMessage(GameMessageType::Quit, senderName)
    {};
    
    ~ClientMessageQuit() = default;
    const std::vector<uint8_t> toBytes() const override;
};

struct ClientMessageChat : public ClientMessage {
    /// Sent by the server if a client sent a message
    const std::string message;
    
    ClientMessageChat(std::string senderName, std::string message) :
        ClientMessage(GameMessageType::Chat, senderName),
        message(message)
    {};
    
    ~ClientMessageChat() = default;
    const std::vector<uint8_t> toBytes() const override;
};

struct ClientMessageMapTileData : public ClientMessage {
    /// Map plane
    MapPlane tileData;
    
    /// Map size
    uint64_t width;
    uint64_t height;
    
    /// Create message from map
    ClientMessageMapTileData(Map& map);
    
    /// Create message from map plane (tileData move constructor)
    ClientMessageMapTileData(MapPlane&& mapPlane, uint64_t width, uint64_t height);
    
    ~ClientMessageMapTileData() = default;
    const std::vector<uint8_t> toBytes() const override;
};

struct ClientMessageMapObjectData : public ClientMessage {
    /// Objects
    std::vector<std::shared_ptr<Object>> objects;
    
    /// Create message from object list
    ClientMessageMapObjectData(const std::vector<std::shared_ptr<Object>>& objects) :
        ClientMessage(GameMessageType::MapObjectData, ""),
        objects(objects)
    {}
    
    ~ClientMessageMapObjectData() = default;
    const std::vector<uint8_t> toBytes() const override;
};

struct ClientMessagePlayerData : public ClientMessage {
    /// Player names
    std::vector<std::string> names;
    
    /// Player positions
    std::vector<std::pair<int, int> > positions;
    
    /// Player levels
    std::vector<int> levels;
    
    /// Create message from players
    ClientMessagePlayerData(std::vector<std::shared_ptr<Player> >& players) :
        ClientMessage(GameMessageType::PlayerData, "")
    {
        for(auto player : players) {
            if(!player->name.empty()) {
                names.push_back(player->name);
                positions.emplace_back(player->playerPositionX, player->playerPositionY);
                levels.push_back(player->level);
            }
        }
    }
    
    /// Create message
    ClientMessagePlayerData(std::vector<std::string>&& names, std::vector<std::pair<int, int> >&& positions, std::vector<int>&& levels) :
        ClientMessage(GameMessageType::PlayerData, ""),
        names(names),
        positions(positions),
        levels(levels)
    {}
    
    ~ClientMessagePlayerData() = default;
    const std::vector<uint8_t> toBytes() const override;
};

struct ClientMessageDoJoin : public ClientMessage {
    /// Sent by the client if the client wants to join the game with a certain
    /// player name
    ClientMessageDoJoin(std::string name) :
        ClientMessage(GameMessageType::DoJoin, name)
    {};
    
    ~ClientMessageDoJoin() = default;
    const std::vector<uint8_t> toBytes() const override;
};

struct ClientMessageDoQuit : public ClientMessage {
    /// Sent by the client if the client wants to quit the game
    ClientMessageDoQuit() :
        ClientMessage(GameMessageType::DoQuit, "")
    {};
    
    ~ClientMessageDoQuit() = default;
};

struct ClientMessageDoChat : public ClientMessage {
    /// Sent by the client if the client wants to send a message
    const std::string message;
    
    ClientMessageDoChat(std::string message) :
        ClientMessage(GameMessageType::DoChat, ""),
        message(message)
    {};
    
    ~ClientMessageDoChat() = default;
    const std::vector<uint8_t> toBytes() const override;
};

#endif
