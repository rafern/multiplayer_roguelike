#include "../networking/Direction.hpp"
#include "Player.hpp"
#include "Inventory.h"

Player::Player(Socket* socket) :
    Socket(std::move(*socket)), // Call move constructor. Source invalidated
    Object('@'),
    level(0)
{
	
    dir = eDirection::STOP;
    speed = 1;
    health = 100;
    attack = 1;
    defense = 1;
    strength = 1;
    speedPotionCooldown = 0;
    healthPotionCooldown = 0;
  
    
    m_type = ObjectType::PLAYER;
}

Player::~Player() {
    if(isValid()) {
        try {
            // Shutdown socket completely
            shutdown(SocketShutdownMode::ShutReadWrite);
        }
        catch(SocketException e) {}; // Ignore network exceptions
    }
}

void Player::itemCheck(int x, int y, Map& map) {
	for(auto it = map.objects.begin(); it != map.objects.end();)
	{
		std::pair<int, int> positionOfObject = (*it)->get_position();
		std::pair<int, int> positionOfPlayer = {x, y};
		if( positionOfObject == positionOfPlayer && (*it)->get_type() == ObjectType::ITEM) {
			inventory.pickUp(std::move(*std::dynamic_pointer_cast<Item>(*it)));
			it = map.objects.erase(it);
		}
		else
			it++;
	}
    
}
//movemen logic of player
//collision detection
//item picking

void Player::playerMovementLogic(Map& map) {
    bool ifBreak = false;
    switch (dir) {
    case eDirection::LEFT:
        for (int i = m_position.first; i >= (m_position.first - speed); i--) {
        	if(i<0)
        	{
                m_position.first = (i + 1);
                ifBreak = true;
                break;
        	}
            if (!(*map.get_map_plane())[m_position.second][i].accesible) {
                m_position.first = (i + 1);
                ifBreak = true;
                break;
            }
            itemCheck(m_position.second, i, map);

        }
        if (ifBreak == false) {
            m_position.first = m_position.first - speed;
        }
        dir = eDirection::STOP;
        break;
    case eDirection::RIGHT:
    	
        for (int i = m_position.first; i <= (m_position.first + speed); i++) {
        	if(i >= map.get_map_size().first)
        	{
                m_position.first = (i - 1);
                ifBreak = true;
                break;
        	}
            if (!(*map.get_map_plane())[m_position.second][i].accesible) {

                m_position.first = (i - 1);
                ifBreak = true;
                break;
            }
            itemCheck(m_position.second, i, map);

        }
        if (ifBreak == false) {
            m_position.first = m_position.first + speed;
        }
        dir = eDirection::STOP;
        break;
    case eDirection::UP:
    	
        for (int i = m_position.second; i >= (m_position.second - speed); i--) {
        	if(i < 0)
        	{
                m_position.second = (i + 1);
                ifBreak = true;
                break;
        	}
            if (!(*map.get_map_plane())[i][m_position.first].accesible) {
                m_position.second = (i + 1);
                ifBreak = true;
                break;
            }
            itemCheck(i, m_position.first, map);
        }
        if (ifBreak == false) {

            m_position.second = m_position.second - speed;
        }
        dir = eDirection::STOP;
        break;
    case eDirection::DOWN:
        for (int i = m_position.second; i <= (m_position.second + speed); i++) {
        	if(i >= map.get_map_size().second)
        	{
                m_position.second = (i - 1);
                ifBreak = true;
                break;
        	}
            if (!(*map.get_map_plane())[i][m_position.first].accesible) {
                m_position.second = (i - 1);
                ifBreak = true;
                break;
            }
            itemCheck(i, m_position.first, map);
        }
        if (ifBreak == false) {
            m_position.second = m_position.second + speed;
        }
        dir = eDirection::STOP;
        break;
    }

    if (speed < 1) {
        speed = 1;
    }
}



void Player::playerAttack(Map& map) {
    std::vector<std::shared_ptr<Object>> objects = map.objects;
    int x = m_position.first;
    int y = m_position.second;

    for (std::shared_ptr<Object> object : objects) {
        if (object->get_type() == ObjectType::ENEMY) {
            std::pair<long, long> objectPosition = object->get_position();
            if (x == objectPosition.first && y == objectPosition.second) {
                //rocked
                //reduce enemy health
            }
            else {
                //u missed
            }
        }
    }
}


