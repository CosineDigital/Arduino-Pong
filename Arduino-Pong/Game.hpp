#pragma once
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>

#include "Player.hpp"
#include "InputData.hpp"
#include "Ball.hpp"
#include "Wall.hpp"

class Game final {

public:
	Player* player1, *player2;
	Ball* ball;
	Wall* bottom, *top;

	std::vector<AABB*> aabbs;

	std::chrono::time_point<std::chrono::steady_clock> t1, t2;
	std::chrono::nanoseconds frameTime;

	std::chrono::milliseconds maxFrameTime;

	float timeStep;

	float playerSpeed;

	bool running;
	int width, height;
	int scoreToWin;

	char ballChar, wallBottomChar, wallTopChar, playerChar;
	 
	Game() : running(false) {

		// get all data
		std::map<std::string, std::string> config;

		std::ifstream file;
		file.open("config.csv");

		std::string name = "";
		std::stringstream ss = {};
		// assume csv structure: name, value
		for (std::string line; std::getline(file, line, '\n');) {
			int i = 0;
			for (std::string value; std::getline(std::stringstream{line}, value, ','); i++) {
				if (i == 0)
					name = value;
				else
					config[name] = value;
			}
			ss.clear();
		}

		// set up the game
		width = stoi(config["game-width-chars"]);
		height = stoi(config["game-height-chars"]);
		scoreToWin = stoi(config["points-to-win"]);

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

		ballChar = stoi(config["ball"]);
		wallTopChar = stoi(config["wall-top"]);
		wallBottomChar = stoi(config["wall-bottom"]);
	}

	void update(InputData data) {
		
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

		ball->y += ball->dy;
		ball->x += ball->dx;

		// handle collisions between ball and the environment
		for (auto* aabb : aabbs) {
			switch (::isColliding(ball, aabb)) {
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
	}

	void draw(void) {
		t1 = std::chrono::high_resolution_clock::now();

		// draw to console









		frameTime = t2 - t1;
		timeStep = (float)frameTime.count() / 1'000'000'000;

		if (frameTime < maxFrameTime) {
			std::this_thread::sleep_for(maxFrameTime - frameTime);
		}

		t2 = std::chrono::high_resolution_clock::now();
	}

	~Game() {
		delete ball;
		for (auto* aabb : aabbs) {
			delete aabb;
		}
	}

private:



};