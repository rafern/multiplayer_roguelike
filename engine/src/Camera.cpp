#include "Camera.h"

Camera::Camera()
{}

Camera::Camera(char blank_char, Map& map, std::pair<long, long> start_position):
	m_blank_char(blank_char), m_map(map), Object('\0', Direction::NORTH, false, start_position)
{}

void Camera::move(const Direction dir){
	std::lock_guard<std::mutex> lock (pos_mutex);
	switch (dir)
	{
	case Direction::SOUTH:
		m_position.second--;
		break;
	case Direction::EAST:
		m_position.first--;
		break;
	case Direction::NORTH:
		m_position.second++;
		break;
	case Direction::WEST:
		m_position.first++;
		break;
	default:
		throw;
		break;
	}
}

void Camera::get_objects_in_range(std::pair<long, long> range_y, std::pair<long, long> range_x){
	for (Object* obj : m_map.objects){
		if (obj->get_visibility() && obj->get_position().second >= range_y.first && 
			obj->get_position().second < range_y.second &&
			obj->get_position().first >= range_x.first &&
			obj->get_position().first < range_x.second) {
				objects_in_range.push_back(obj);
			}
	}
}

std::string Camera::get_to_render()	{
	std::string map_to_render("");

	MapPlane* plane = m_map.get_map_plane();
	std::pair<unsigned long, unsigned long> map_size = m_map.get_map_size();

	size_t x = 0, y = 0;
	Color last_color = Color::NO_COLOR;

	get_objects_in_range({-1 * (RENDER_HEIGHT / 2) + m_position.second,  RENDER_HEIGHT / 2 + m_position.second},
						 {-1 * (RENDER_WIDTH / 2) + m_position.first,    RENDER_WIDTH / 2 + m_position.first});

	std::lock_guard<std::mutex> lock (pos_mutex);

	for (int i = -1 * (RENDER_HEIGHT / 2) + m_position.second; i < RENDER_HEIGHT / 2 + m_position.second; i++)
	{
		for (int j = -1 * (RENDER_WIDTH / 2) + m_position.first; j < RENDER_WIDTH / 2 + m_position.first; j++)
		{
			if (i >= 0 && j >= 0 && i < map_size.second && j < map_size.first)
			{
				if((*plane)[i][j].color!=last_color){
					last_color = (*plane)[i][j].color;
					map_to_render.append("\033[0m");
					map_to_render.append(std::string("\033[" + std::to_string(static_cast<int>((*plane)[i][j].color) )+ "m"));
				}
				bool object = false;
				for (Object* obj : objects_in_range){
				if (obj->get_visibility() && obj->get_position().second == i && obj->get_position().first == j){
					map_to_render.append(std::string("\033[" + std::to_string(static_cast<int>(obj->get_color()) ) + "m"));
					map_to_render += obj->get_char();
					map_to_render.append(std::string("\033[" + std::to_string(static_cast<int>(last_color) )+ "m"));
					object = true;
				}
				}
				if(!object)
					map_to_render += (*plane)[i][j].character;
			}
			else
			{
				if(last_color != Color::NO_COLOR){
					last_color = Color::NO_COLOR;
					map_to_render.append("\033[0m");
				}
				map_to_render+=m_blank_char;
			}
			
			x++;
		}
		map_to_render += '\n';
		y++;
		x = 0;
	}
	map_to_render.append("\033[0m");

	plane = nullptr;
	delete plane;
	objects_in_range.clear();
	return {map_to_render};
}