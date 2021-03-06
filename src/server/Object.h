#ifndef ROGUELIKE_OBJECT_H_INCLUDED
#define ROGUELIKE_OBJECT_H_INCLUDED
#include <utility>
#include "../client/Formatting.hpp"
#include "../client/Texture.h"

enum class Direction {
	NORTH = 0,
	EAST,
	SOUTH,
	WEST
};

enum class ObjectType {
    GENERIC = 0,
    ENEMY = 1,
    PLAYER = 2,
    ITEM = 3
};

class Object
{
public:
	virtual ~Object() = default;

	Object(const char character, Direction direction, bool visibility, std::pair<int, int> start_position, Formating formating, Texture texture, ObjectType type) :
		m_character(character), m_dir(direction), m_visibility(visibility), m_position(std::move(start_position)), m_formating(formating), m_texture(texture), m_type(type) {}
	Object(const char character, Direction direction, bool visibility, std::pair<int, int> start_position, Formating formating, Texture texture) :
		Object(character, direction, visibility, start_position, formating, {}, ObjectType::GENERIC) {}
	Object(const char character, Direction direction, bool visibility, std::pair<int, int> start_position, Formating formating) :
		Object(character, direction, visibility, start_position, formating, {}) {}
	Object(const char character, Direction direction, bool visibility, std::pair<int, int> start_position) : 
		Object(character, direction, visibility, start_position, {Color::WHITE, Color::BLACK}) {}
	Object(const char character, Direction direction, bool visibility) : 
		Object(character, direction, visibility, { 5,5 }) {}
	Object(const char character, Direction direction) : 
		Object(character, direction, true) {}
	Object(const char character) : 
		Object(character, Direction::NORTH) {}
	Object() : 
		Object('x') {}

	char get_char();

	Formating get_formating() const;

	bool get_visibility() const;

	std::pair<long, long> get_position() const;

	Texture get_texture() const;
    
    Direction get_direction() const;
    
    ObjectType get_type() const;

	virtual void move(const Direction dir);

	virtual void update() {
		
	}

	void set_position(const std::pair<int, int> new_position);

	bool is_player = false;
	
protected:
	// graphical representation of object;
	char m_character;
	// first - x position, second - y position
	std::pair<int, int> m_position;
	// actual direction that object is facing
	Direction m_dir;
	// define if object is visible
	bool m_visibility;
	// color of the object
	Formating m_formating;
	Texture m_texture;
    // Type of object
    ObjectType m_type;
};

#endif
