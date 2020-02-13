#pragma once
#include <utility>
#include "Commons.h"

class Object
{
public:
	virtual ~Object() = default;

	Object(const char character, Direction direction, bool visibility, std::pair<long, long> start_position, Color color) :
		m_character(character), m_dir(direction), m_visibility(visibility), m_position(std::move(start_position)), m_color(color) {}
	Object(const char character, Direction direction, bool visibility, std::pair<long, long> start_position) : 
		Object(character, direction, visibility, start_position, Color::LIGHT_AQUA) {}
	Object(const char character, Direction direction, bool visibility) : 
		Object(character, direction, visibility, { 0,0 }) {}
	Object(const char character, Direction direction) : 
		Object(character, direction, true) {}
	Object(const char character) : 
		Object(character, Direction::NORTH) {}
	Object() : 
		Object('x') {}

	char get_char() const;

	Color get_color() const;

	bool get_visibility() const;

	std::pair<long, long> get_position() const;

	void move(const Direction dir);

	virtual void update() {
		
	}

	void set_position(const std::pair<long, long> new_position);
	
protected:
	// graphical representation of object;
	char m_character;
	// first - x position, second - y position
	std::pair<long, long> m_position;
	// actual direction that object is facing
	Direction m_dir;
	// define if object is visible
	bool m_visibility;
	// color of the object
	Color m_color;
};