#pragma once
#include <Windows.h>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>
#include <conio.h>

#include "Player.hpp"
#include "InputData.hpp"
#include "Ball.hpp"
#include "Wall.hpp"

class Game final {

public:
	bool running;

public:
	Game() : running(true) {

		// get all data
		std::map<std::string, std::string> config;

		std::ifstream file;
		file.open("config.csv");

		std::string name = "";
		// assume csv structure: name, value
		for (std::string line; std::getline(file, line, '\n');) {
			if (line.substr(0, 2) == "//")
				continue;

			int i = 0;
			std::string value;
			std::stringstream ss(line);
			while (std::getline(ss, value, ',')) {
				if (i++ == 0)
					name = value;
				else
					config[name] = value;
			}
			ss.clear();
		}

		// set up the game
		gameWidthChars = stoi(config["game-width-chars"]);
		gameHeightChars = stoi(config["game-height-chars"]);
		scoreToWin = stoi(config["points-to-win"]);

		charBuffer = new char[gameWidthChars * gameHeightChars];
		std::memset(charBuffer, 33, gameWidthChars * gameHeightChars);

		maxFrameTime = std::chrono::milliseconds(stoi(config["millis-per-frame"]));
		playerSpeed = stof(config["player-speed"]);

		ball = new Ball();

		ball->aabb.width = stof(config["ball-radius"]);
		ball->aabb.height = stof(config["ball-radius"]);

		player1 = new Player();

		player1->width = stof(config["player-width"]);
		player1->height = stof(config["player-height"]);
		player1->x = - stof(config["player-offset"]);

		player2 = new Player();

		player2->width = stof(config["player-width"]);
		player2->height = stof(config["player-height"]);
		player1->x = + stof(config["player-offset"]);

		top = new Wall();

		top->ypos = + stof(config["wall-offset"]);
		top->width = stof(config["wall-width"]);
		top->height = stof(config["wall-height"]);

		bottom = new Wall();

		bottom->ypos = -stof(config["wall-offset"]);
		bottom->width = stof(config["wall-width"]);
		bottom->height = stof(config["wall-height"]);

		aabbs = { player1, player2, top, bottom };
		
		ballChar = (config["ball"])[0];
		wallTopChar = (config["wall-top"])[0];
		wallBottomChar = (config["wall-bottom"])[0];

		std::iostream::sync_with_stdio(0);
	}

	void update(InputData data) {
		
		t1 = std::chrono::high_resolution_clock::now();

		if (data.resetPressed) {
			running = false;
		}

		// update velocities

		// player 1, the left player
		if (data.p1UpPressed) {
			player1->dy = playerSpeed;
		}
		if (data.p1DownPressed) {
			player1->dy = -playerSpeed;
		}

		// player 2, the right player
		if (data.p2UpPressed) {
			player2->dy = playerSpeed;
		}
		if (data.p2DownPressed) {
			player2->dy = -playerSpeed;
		}

		// update positions
		player1->y += player1->dy * timeStep;
		player2->y += player2->dy * timeStep;

		ball->y += ball->dy * timeStep;
		ball->x += ball->dx * timeStep;

		// handle collisions between ball and the environment
		for (auto* aabb : aabbs) {
			switch (ball->isColliding(aabb)) {
				case ColDir::NONE:
					break;
				case ColDir::RIGHT:
					ball->dx *= -1;
					break;
				case ColDir::TOP:
					ball->dy *= -1;
					break;
				case ColDir::LEFT:
					ball->dx *= -1;
					break;
				case ColDir::BOTTOM:
					ball->dy *= -1;
					break;
			}
		}

		this->draw();

		t2 = std::chrono::high_resolution_clock::now();

		frameTime = t2 - t1;
		timeStep = (float)std::chrono::duration_cast<std::chrono::milliseconds>(frameTime).count() / 1000;

		if (frameTime < maxFrameTime) {
			std::this_thread::sleep_for(maxFrameTime - frameTime);
		}
	}
	
	~Game() {
		delete ball;
		for (auto* aabb : aabbs) {
			delete aabb;
		}

		delete[] charBuffer;
	}

private:

	void draw(void) {
		system("cls");
		std::puts(charBuffer);
	}

private:

	Player* player1, * player2;
	Ball* ball;
	Wall* bottom, * top;

	std::vector<AABB*> aabbs;

	std::chrono::time_point<std::chrono::steady_clock> t1, t2;
	std::chrono::nanoseconds frameTime;

	std::chrono::milliseconds maxFrameTime;

	int gameHeightChars, gameWidthChars;
	int scoreToWin;

	float timeStep;

	float playerSpeed;

	char* charBuffer;


	char ballChar, wallBottomChar, wallTopChar, playerChar;
};