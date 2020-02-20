#pragma once
#include <time.h>
#include <iostream>
#include "Commons.h"
#include <thread>
#include "Camera.h"
#if defined(unix) || defined(__unix) || defined(__unix__)
#include <unistd.h>
#include <termios.h>
#endif

// allows to fast writing data into console
class Renderer
{
public:
	// change to false to end rendering
	bool b_render = true;

	Renderer(Camera* cam) : m_cam(cam) {
	}

	// prints given data into the output stream
	void render() {
		// clear the screen
		std::cout << "\x1B[2J\x1B[H";
		// go to new line and disable cursor
		std::cout << "\e[?25l";
		while (b_render) {
			// go to (0,0) position
			std::cout << "\033[0;0f" << m_cam->get_to_render3D() << "------------\n\033[0m";
			std::cout<< m_cam->get_minimap_to_render() << "\033[0m";
		}
		std::cout << "\033[0m";
		// re-enable cursor
		std::cout << "\x1B[2J\x1B[H";
		// clear console
	}

	std::thread spawn() {
		return std::thread(&Renderer::render, this);
	}

	// sets console window title
	void set_title(const char* title) {
	//	SetConsoleTitle(title);
		title = nullptr;
		delete title;
	}

#if defined(unix) || defined(__unix) || defined(__unix__)
	//Adapted from https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux
	static char getch(void) {
		char buf = 0;
		struct termios old = {0};
		fflush(stdout);
		if(tcgetattr(0, &old) < 0)
			perror("tcsetattr()");
		old.c_lflag &= ~ICANON;
		old.c_lflag &= ~ECHO;
		old.c_cc[VMIN] = 1;
		old.c_cc[VTIME] = 0;
		if(tcsetattr(0, TCSANOW, &old) < 0)
			perror("tcsetattr ICANON");
		if(read(0, &buf, 1) < 0)
			perror("read()");
		old.c_lflag |= ICANON;
		old.c_lflag |= ECHO;
		if(tcsetattr(0, TCSADRAIN, &old) < 0)
			perror("tcsetattr ~ICANON");
		//printf("%c\n", buf);
		return buf;
	}
	//End of Adapted
#endif

	~Renderer(){
		//delete m_cam;
		//m_cam = nullptr;
	}
private:
	Camera* m_cam;
};
