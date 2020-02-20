#include "Camera.h"
#include <iostream>

Camera::Camera()
{}

Camera::Camera(char blank_char, Map& map, std::pair<long, long> start_position):
	m_blank_char(blank_char), m_map(map), Object('\0', Direction::NORTH, false, start_position)
{}

void Camera::move(const Direction dir){
	std::lock_guard<std::mutex> lock (pos_mutex);

	switch (dir)
	{
	case Direction::NORTH:
		m_position.first += sinf(m_angle);
		m_position.second += cosf(m_angle);
		if(m_position.first)
		break;
	case Direction::SOUTH:
		m_position.first -= sinf(m_angle);
		m_position.second -= cosf(m_angle);
	default:
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

std::string Camera::get_minimap_to_render()	{
	// keeps generated map string
	std::string map_to_render = "";

	// plane of the map
	MapPlane* plane = m_map.get_map_plane();
	// size of the map
	std::pair<unsigned long, unsigned long> map_size = m_map.get_map_size();

	// color of last cell
	Color last_fg_color = Color::NO_COLOR;
	Color last_bg_color = Color::NO_COLOR;

	// get objects in range of camera
	get_objects_in_range({-1 * (MINIMAP_HEIGHT / 2) + m_position.second,  MINIMAP_HEIGHT / 2 + m_position.second},
						 {-1 * (MINIMAP_WIDTH / 2) + m_position.first,    MINIMAP_WIDTH / 2 + m_position.first});

	// lock camera position to avoid changing position during rendering
	std::lock_guard<std::mutex> lock (pos_mutex);

	for (int i = -1 * (MINIMAP_HEIGHT / 2) + m_position.second; i < MINIMAP_HEIGHT / 2 + m_position.second; i++)
	{
		for (int j = -1 * (MINIMAP_WIDTH / 2) + m_position.first; j < MINIMAP_WIDTH / 2 + m_position.first; j++)
		{
			if (i >= 0 && j >= 0 && i < map_size.second && j < map_size.first)
			{
				// check if last current cell is in the same color as before
				if((*plane)[i][j].formating.text_color!=last_fg_color ||
					(*plane)[i][i].formating.background_color!=last_bg_color){
					// set last_fg_color to current color
					last_fg_color = (*plane)[i][j].formating.text_color;
					last_bg_color = (*plane)[i][j].formating.background_color;
					// set color code to current color
					// \033[3;42;30m
					map_to_render.append(std::string("\033[3;" + std::to_string(static_cast<int>((*plane)[i][j].formating.background_color)+10 ) + ";" +
					  std::to_string(static_cast<int>((*plane)[i][j].formating.text_color) ) + "m"));
				}
				// flag to check if on current cell is an object
				bool object = false;
				// check all objects in range if there is a one on current position
				for (Object* obj : objects_in_range){
				if (obj->get_visibility() && obj->get_position().second == i && obj->get_position().first == j){
					map_to_render.append(std::string("\033[3;" + std::to_string(static_cast<int>(obj->get_formating().background_color)+10 ) + ";" +
					  std::to_string(static_cast<int>(obj->get_formating().text_color) ) + "m"));
					map_to_render += obj->get_char();
					map_to_render.append(std::string("\033[" + std::to_string(static_cast<int>(last_bg_color)+10 )+ ";" + std::to_string(static_cast<int>(last_fg_color)) +"m"));
					object = true;
				}
				}
				if(!object)
					map_to_render += (*plane)[i][j].character;
			}
			else
			{
				if(last_fg_color != Color::NO_COLOR){
					last_fg_color = Color::NO_COLOR;
					map_to_render.append("\033[0m");
				}
				map_to_render+=' ';
			}
		}
		map_to_render += '\n';
	}
	map_to_render.append("\033[0m");
	objects_in_range.clear();
	return {map_to_render};
}

// some parts of code adapted from https://github.com/OneLoneCoder/CommandLineFPS/blob/master/CommandLineFPS.cpp
std::string Camera::get_to_render3D() {
	//lock the position values to avoid changing it while rendering a frame
	std::lock_guard<std::mutex> lock (pos_mutex);
	// plane of the map
	MapPlane* plane = m_map.get_map_plane();
	// size of the map
	std::pair<unsigned long, unsigned long> map_size = m_map.get_map_size();

	std::vector<std::string> vec_map_to_render(RENDER_WIDTH, std::string(RENDER_HEIGHT, ' '));

	// cast a ray for every column in a screen
	for(size_t i = 0; i<RENDER_WIDTH; i++){
		// For each column, calculate the projected ray angle into world space
		float fRayAngle = (m_angle - m_fov/2.0f) + ((float)i / (float)RENDER_WIDTH) * m_fov;

		
		float step = 0.1f; // step for incrementing distance of ray				
		float dist = 0.0f; // distance of ray

		bool obstacle_flag = false;		// true if there is a wall on the path of ray
		bool boundary_flag = false;		// true if ray hit between the walls

		float fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
		float fEyeY = cosf(fRayAngle);

		while(!obstacle_flag && dist<RENDER_DEPTH){
			// increment the ray distance
			dist += step;

			// current position of ray
			int ray_pos_x = static_cast<int>(static_cast<float>(m_position.first) + fEyeX * dist);
			int ray_pos_y = static_cast<int>(static_cast<float>(m_position.second) + fEyeY * dist);

			// check if ray is still in the map
			if (ray_pos_x < 0 || ray_pos_x >= map_size.first || ray_pos_y < 0 || ray_pos_y >= map_size.second) {
				break;
			}
			// check if ray hitted the wall
			else if(!(*plane)[ray_pos_x][ray_pos_y].accesible) {
				obstacle_flag = true;
				char render_char;
				std::string color_code;
				// set character equivalent to distance from wall
				if (dist <= RENDER_DEPTH / 4.0f){
					render_char = '#';	
				}
				else if (dist < RENDER_DEPTH / 3.0f){
					render_char = '8';
					}
				else if (dist < RENDER_DEPTH / 2.0f){
					render_char = '-';
				}
				else if (dist < RENDER_DEPTH){
					render_char = '.';
				}
					
				std::vector<std::pair<float, float>> p;

				// Test each corner of hit tile, storing the distance from
				// the player, and the calculated dot product of the two rays
				for (int tx = 0; tx < 2; tx++)
					for (int ty = 0; ty < 2; ty++)
					{
						// Angle of corner to eye
						float vy = static_cast<float>(ray_pos_y) + ty - m_position.second;
						float vx = static_cast<float>(ray_pos_x) + tx - m_position.first;
						float d = sqrt(vx*vx + vy*vy); 
						float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
						p.push_back(std::make_pair(d, dot));
					}
				// Sort Pairs from closest to farthest
				std::sort(p.begin(), p.end(), [](const std::pair<float, float> &left, const std::pair<float, float> &right) {return left.first < right.first; });
					
				float fBound = 0.01;
				if (acos(p.at(0).second) < fBound) boundary_flag = true;
				if (acos(p.at(1).second) < fBound) boundary_flag = true;
				if (acos(p.at(2).second) < fBound) boundary_flag = true;

				unsigned short len_of_column = static_cast<int>(RENDER_HEIGHT-dist*2.0f);
				if(len_of_column>=RENDER_HEIGHT) len_of_column = RENDER_HEIGHT-1;

					if(!boundary_flag) {
						vec_map_to_render[i].replace((RENDER_HEIGHT-len_of_column)/2,
						len_of_column,
						std::string(len_of_column,
						render_char));
						(*plane)[ray_pos_x][ray_pos_y].character = 'x';
					}
				}
			}
	}

	// convert frame from vector to string
	std::string str_map_to_render;
	bool color_flag = false;
	int last_color = -1;
	for(int i = 0; i<RENDER_HEIGHT; i++){
		for(int j = 0; j<RENDER_WIDTH; j++){
			str_map_to_render += vec_map_to_render[j][i];
		}
		str_map_to_render+='\n';
	}
	str_map_to_render.append(std::to_string(m_angle));
	return str_map_to_render;
}
// end of adapted code