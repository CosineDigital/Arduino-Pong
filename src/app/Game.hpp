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
#include <random>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <KHR/khrplatform.h>
#include <glm/glm.hpp>

#include "../input/InputData.hpp"
#include "../graphics/renderer.h"
#include "../core/Ball.hpp"
#include "../input/SerialPort.hpp"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, int button, int action, int bits);
static void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int bits);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void cursor_callback(GLFWwindow* window, double xpos, double ypos);
static void glfw_error_callback(int error, const char* description);

class Game final {

public:
	Game() {
		ball = new Ball();

		player1 = new AABB();
		player2 = new AABB();
		goal1 = new AABB();
		goal2 = new AABB();

		top = new AABB();
		bottom = new AABB();

		port = new SerialPort();

		aabbs = { player1, player2, top, bottom, goal1, goal2 };
	}

	int init() {

		// open config file
		std::ifstream file("resources/config.csv");
		if (file.is_open()) {
			// get all data
			std::map<std::string, std::string> config;
			// assume csv structure: name, value
			for (std::string line; std::getline(file, line, '\n');) {
				// ignore comments
				if (line.substr(0, 2) == "//")
					continue;

				int i = 0;
				std::string name, value;
				std::stringstream ss(line);
				while (std::getline(ss, value, ',')) {
					if (i++ == 0)
						name = value;
					else
						config[name] = value;
				}
				ss.clear();
			}

			// open port
			// Set up the port for comminicating with the arduino
			char portName[16] = { 0 };
			sprintf_s(portName, "\\\\.\\COM%i", stoi(config["arduino-port"]));

			if (port->init(portName)) {

				// set up the game
				width = stoi(config["game-width"]);
				height = stoi(config["game-height"]);
				pointsToWin = stoi(config["points-to-win"]);
				pointsPerGoal = stoi(config["points-per-goal"]);

				maxFrameTime = std::chrono::milliseconds(stoi(config["millis-per-frame"]));
				playerSpeed = stof(config["player-speed"]);
				ballSpeed = stof(config["ball-speed"]);
				ballSpeedMultiplier = stof(config["ball-speed-multiplier"]);

				ball->dim.y = stof(config["ball-radius"]);
				ball->dim.x = stof(config["ball-radius"]);

				player1->dim.x = stof(config["player-width"]);
				player1->dim.y = stof(config["player-height"]);
				player1->pos.x = -stof(config["player-offset"]);

				player2->dim.x = stof(config["player-width"]);
				player2->dim.y = stof(config["player-height"]);
				player2->pos.x = +stof(config["player-offset"]);

				// set up walls

				top->pos.y = +stof(config["wall-offset"]);
				top->dim.x = stof(config["wall-width"]);
				top->dim.y = stof(config["wall-height"]);

				bottom->pos.y = -stof(config["wall-offset"]);
				bottom->dim.x = stof(config["wall-width"]);
				bottom->dim.y = stof(config["wall-height"]);

				bool spawnOtherWalls = stoi(config["spawn-other-walls"]);
				float otherWallOffsetX = stof(config["other-wall-offset-x"]);
				float otherWallOffsetY = stof(config["other-wall-offset-y"]);
				float otherWallWidth = stof(config["other-wall-width"]);
				float otherWallHeight = stof(config["other-wall-height"]);
				if (spawnOtherWalls) {
					// spawn four walls in the corners of the game
					for (int i = -1; i < 2; i += 2) {
						for (int j = -1; j < 2; j += 2) {
							AABB* otherWall = new AABB();

							otherWall->dim = { otherWallWidth, otherWallHeight };
							otherWall->pos = { otherWallOffsetX * i, otherWallOffsetY * j};
							aabbs.push_back(otherWall);
						}
					}
				}

				// pause time between goals and after reset
				resetPauseTime = stof(config["reset-pause-time"]);
				// by default have a pause before starting the game
				resetPauseTimer = resetPauseTime;

				// goal 1
				goal1->is_trigger = true;
				goal1->trigger_callback = [&]() {
					player2Points += pointsPerGoal;
					onGoal();
				};
				goal1->dim.x = stof(config["goal-width"]);
				goal1->dim.y = stof(config["goal-height"]);
				goal1->pos.x = -stof(config["goal-offset"]);

				// goal 2
				goal2->is_trigger = true;
				goal2->trigger_callback = [&]() {
					player1Points += pointsPerGoal;
					onGoal();
				};
				goal2->dim.x = stof(config["goal-width"]);
				goal2->dim.y = stof(config["goal-height"]);
				goal2->pos.x = +stof(config["goal-offset"]);


				// set up ball
				// give a random velocity

				float a = randomAngle();
				ball->vel = {ballSpeed * std::cosf(a), ballSpeed * std::sinf(a)};


				// set up opengl context


				glfwInit();
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
				glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
				glfwWindowHint(GLFW_REFRESH_RATE, 60);

				glfwWindow = glfwCreateWindow(width, height, "Arduino-Pong", 0, 0);

				if (glfwWindow) {
					glfwSetWindowUserPointer(glfwWindow, this);
					glfwMakeContextCurrent(glfwWindow);
					glfwSetFramebufferSizeCallback(glfwWindow, ::framebuffer_size_callback);
					glfwSetMouseButtonCallback(glfwWindow, ::mouse_callback);
					glfwSetKeyCallback(glfwWindow, ::keyboard_callback);
					glfwSetScrollCallback(glfwWindow, ::scroll_callback);
					glfwSetCursorPosCallback(glfwWindow, ::cursor_callback);
					glfwSetErrorCallback(::glfw_error_callback);

					if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
						// set up renderer
						renderer = new Renderer();
						camera = new Camera({ 0, 0, 0 }, { 0, 1, 0 }, -90.f, 0.f, 0.01f);
						// assume that the height is 1
						camera->h = 1 * 2;
						camera->w = ((float)width / (float)height) * 2.f;

						// success
						return 1;
					}
					else {
						std::cerr << "Could not initialize Glad\n";
						return 0;
					}
				}
				else {
					std::cerr << "Could not create a GLFWWindow\n";
					return 0;
				}
			}
			else {
				std::cerr << "Could not initialize the port\n";
				return 0;
			}
		}
		else {
			std::cerr << "Could not open config file\n";
			return 0;
		}
	}

	static constexpr unsigned char REQUEST_READY = 1 << 7;
	static constexpr unsigned char REQUEST_HOLD  = 1 << 6;

	void run() {

		while (!glfwWindowShouldClose(glfwWindow)) {

			t1 = std::chrono::high_resolution_clock::now();

			// only update the game if the game is not paused
			if (resetPauseTimer < 0) {
				
				unsigned char response = 0;

				// send a ready request to the arduino to send its data
				if (port->writeByte(REQUEST_READY)) {

					// then read the reponsse

					if (!port->readByte(&response)) {
						response = 0;
					}
				}

				this->update(::byteToInputData(response));
			}
			else {
				// send a hold request to the arduino to not send data
				port->writeByte(REQUEST_HOLD);

				resetPauseTimer -= timeStep;
			}

			// always draw everything
			this->draw();

			t2 = std::chrono::high_resolution_clock::now();

			frameTime = t2 - t1;
			timeStep = (float)std::chrono::duration_cast<std::chrono::milliseconds>(frameTime).count() / 1000;
		}
	}
	
	~Game() {
		delete ball;
		for (auto* aabb : aabbs) {
			delete aabb;
		}

		delete camera, renderer, port;

		glfwTerminate();
	}

private:

	void update(InputData data) {

		if (data.Reset) {
			// reset points and reset the game
			player1Points = player2Points = 0;
			Reset();
		}

		// update velocities

		float multiplier = 1;

		// player 1, the left player
		// first check if we can move up anymore
		if (player1->pos.y <  1 - top->dim.y - player1->dim.y &&
			player1->pos.y > -1 + top->dim.y + player1->dim.y)
		{
			multiplier = 1;
		}
		else {
			multiplier = -1;
		}

		// update the player accordingly
		if (data.P1Up) {
			player1->pos.y += playerSpeed * timeStep * multiplier;
		}
		if (data.P1Down) {
			player1->pos.y -= playerSpeed * timeStep * multiplier;
		}
		

		// player 2, the right player
		// first check if we can move up anymore
		if (player2->pos.y <  1 - top->dim.y - player1->dim.y &&
			player2->pos.y > -1 + top->dim.y + player1->dim.y) {
			multiplier = 1;
		}
		else {
			multiplier = -1;
		}

		// update the player accordingly
		if (data.P2Up) {
			player2->pos.y += playerSpeed * timeStep;
		}
		if (data.P2Down) {
			player2->pos.y -= playerSpeed * timeStep;
		}


		// handle collisions between ball and the environment
		for (auto* aabb : aabbs) {

			Collision collision = ball->checkCollision(aabb, timeStep);

			// same as collisiondirection == none
			if (collision.time < 1.f) {
				// if it's a goal, then call the callback
				if (aabb->is_trigger) {
					aabb->trigger_callback();
					continue;
				}

				// else, get the direction and handle the ball
				switch (collision.direction) {
					case CollisionDirection::RIGHT:
						ball->vel.x *= -1;
						break;
					case CollisionDirection::BOTTOM:
						ball->vel.y *= -1;
						break;
					case CollisionDirection::LEFT:
						ball->vel.x *= -1;
						break;
					case CollisionDirection::TOP:
						ball->vel.y *= -1;
						break;
				}

				// TODO: more advanced collision and angles


				// on collision increase the speed
				ball->vel *= ballSpeedMultiplier;
			}
		}
		
		// update ball
		ball->pos += ball->vel * timeStep;
	}

	void draw(void) {

		glClear(GL_COLOR_BUFFER_BIT);

		// create objects on screen:
		renderer->bufferQuad(ball->pos, ball->dim);

		for (auto* aabb : aabbs) {
			// don't draw goals
			if (aabb->is_trigger) {
				continue;
			}
			renderer->bufferQuad(aabb->pos, aabb->dim);
		}

		renderer->render(camera);

		renderer->clear();

		glfwSwapBuffers(glfwWindow);
		glfwPollEvents();
	}

	inline float randomUnit() noexcept {
		return randomEngine(gen);
	}

	inline float randomAngle() noexcept {
		return randomUnit() * 6.283185307179586476925286766559f;
	}

	void onGoal() noexcept {
		if (player1Points >= pointsToWin) {
			std::cout << "Player 1 wins\n";
			player1Points = player2Points = 0;
		}
		else if (player2Points >= pointsToWin) {
			std::cout << "Player 2 wins\n";
			player1Points = player2Points = 0;
		}
		Reset();
	}

	void Reset() {
		// reset the game, wait then place the ball again
		ball->pos = glm::vec2{};

		float a = randomAngle();
		ball->vel = { ballSpeed * std::cosf(a), ballSpeed * std::sinf(a) };

		player1->pos.y = player2->pos.y = 0;

		timeStep = 0;
		resetPauseTimer = resetPauseTime;
	}

private:

	std::random_device rd;
	std::mt19937 gen = std::mt19937(rd());
	std::uniform_real_distribution<float> randomEngine = std::uniform_real_distribution<float>(0.f, 1.f);

	AABB* player1, * player2;
	Ball* ball;
	AABB* bottom, * top;
	AABB* goal1, * goal2;

	int player1Points, player2Points;

	std::vector<AABB*> aabbs;

	std::chrono::time_point<std::chrono::steady_clock> t1, t2;
	std::chrono::nanoseconds frameTime;

	std::chrono::milliseconds maxFrameTime;

	int height, width;
	int pointsToWin, pointsPerGoal;

	float resetPauseTime;
	float resetPauseTimer;

	float timeStep;

	float playerSpeed;
	float ballSpeed;
	float ballSpeedMultiplier;

	SerialPort* port;
	GLFWwindow* glfwWindow;
	Renderer* renderer;
	Camera* camera;

public:

	void onMouseEvent(GLFWwindow* window, int button, int action, int bits) noexcept {
	}

	void onKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int bits) noexcept {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) { glfwSetWindowShouldClose(window, GL_TRUE); }
	}

	void onScrollEvent(GLFWwindow* window, double xoffset, double yoffset) noexcept {
	}

	void onCursorEvent(GLFWwindow* window, double xpos, double ypos) noexcept {
	}

	void onWindowResize(GLFWwindow* window, int width, int height) noexcept {
	}
};


// other callbacks
static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	((Game*)glfwGetWindowUserPointer(window))->onWindowResize(window, width, height);
	glViewport(0, 0, width, height);
}
static void mouse_callback(GLFWwindow* window, int button, int action, int bits) {
	((Game*)glfwGetWindowUserPointer(window))->onMouseEvent(window, button, action, bits);
}
static void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int bits) {
	((Game*)glfwGetWindowUserPointer(window))->onKeyboardEvent(window, key, scancode, action, bits);
}
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	((Game*)glfwGetWindowUserPointer(window))->onScrollEvent(window, xoffset, yoffset);
}
static void cursor_callback(GLFWwindow* window, double xpos, double ypos) {
	((Game*)glfwGetWindowUserPointer(window))->onCursorEvent(window, xpos, ypos);
}
static void glfw_error_callback(int error, const char* description)
{
	std::cerr << "GLFW Error - " << error << " - " << description << '\n';
}