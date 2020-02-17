#include "player.hpp"
using namespace std;

Player::Player(Socket* socket) :
    Socket(std::move(*socket)) // Call move constructor. Source invalidated
{
	dir = STOP;
	speed = 1;
	playerPositionX = 10;
	playerPositionY = 15;
	obstaclePositionY = 15;
	obstaclePositionX = 15;
	potionPositionX = 12;
	potionPositionY = 2;
}


void Player::potionCheck(int axisValue1, int axisValue2, vector <vector<char>>& map) {
	if (map[axisValue1][axisValue2] == 'P') {
		speed++;
		potionPositionX = rand() % 40 + 1;
		potionPositionY = rand() % 20 + 1;
	}
}

void Player::playerMovementLogic(vector <vector<char>>& map) {
	bool ifBreak = false;
	switch (dir) {
	case LEFT:
		for (int i = playerPositionX; i >= (playerPositionX - speed); i--) {
			if (map[playerPositionY][i] == '#') {
				playerPositionX = (i + 1);
				ifBreak = true;
				break;
			}
			potionCheck(playerPositionY, i);

		}
		if (ifBreak == false) {
			playerPositionX = playerPositionX - speed;
		}
		dir = STOP;
		break;
	case RIGHT:
		for (int i = playerPositionX; i <= (playerPositionX + speed); i++) {
			if (map[playerPositionY][i] == '#') {

				playerPositionX = (i - 1);
				ifBreak = true;
				break;
			}
			potionCheck(playerPositionY, i);

		}
		if (ifBreak == false) {
			playerPositionX = playerPositionX + speed;
		}
		dir = STOP;
		break;
	case UP:
		for (int i = playerPositionY; i >= (playerPositionY - speed); i--) {
			if (map[i][playerPositionX] == '#') {
				playerPositionY = (i + 1);
				ifBreak = true;
				break;
			}
			potionCheck(i, playerPositionX);
		}
		if (ifBreak == false) {

			playerPositionY = playerPositionY - speed;
		}
		dir = STOP;
		break;
	case DOWN:
		for (int i = playerPositionY; i <= (playerPositionY + speed); i++) {
			if (map[i][playerPositionX] == '#') {
				playerPositionY = (i - 1);
				ifBreak = true;
				break;
			}
			potionCheck(i, playerPositionX);
		}
		if (ifBreak == false) {
			playerPositionY = playerPositionY + speed;
		}
		dir = STOP;
		break;
	}

	if (playerPositionX > 38) {
		playerPositionX = 38;
	}
	if (playerPositionX < 1) {
		playerPositionX = 1;
	}
	if (playerPositionY > 18) {
		playerPositionY = 18;
	}
	if (playerPositionY < 1) {
		playerPositionY = 1;
	}

	if (speed < 1) {
		speed = 1;
	}

}


void Player::inputHandling(char new_dir)
{

	switch (new_dir)
	{
	case 'a':
		dir = LEFT;
		break;
	case 's':
		dir = DOWN;
		break;
	case 'd':
		dir = RIGHT;
		break;
	case 'w':
		dir = UP;
		break;
	case '-':
		speed--;
		break;
	}

}


