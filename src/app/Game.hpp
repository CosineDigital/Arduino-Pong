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

#include "../input/InputData.hpp"
#include "../graphics/renderer.h"
#include "../core/Ball.hpp"
#include "../input/SerialPort.hpp"
#include "C:\C++ Libraries\glad\include\glad\glad.h"
#include "C:\C++ Libraries\glfw-3.3.2.bin.WIN64\glfw-3.3.2.bin.WIN64\include\GLFW\glfw3.h"
#include "C:\C++ Libraries\glad\include\KHR\khrplatform.h"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, int button, int action, int bits);
static void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int bits);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void cursor_callback(GLFWwindow* window, double xpos, double ypos);
static void glfw_error_callback(int error, const char* description);

class Game final {

public:
	int err;

public:
	Game() {
		ball = new Ball();

		player1 = new AABB();
		player2 = new AABB();
		goal1 = new AABB();
		goal2 = new AABB();

		top = new AABB();
		bottom = new AABB();

		aabbs = { player1, player2, top, bottom };
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
			sprintf_s(portName, R"(\\.\COM%i)", stoi(config["arduino-port"]));

			if (port.init(portName)) {

				// set up the game
				width = stoi(config["game-width"]);
				height = stoi(config["game-height"]);
				pointsToWin = stoi(config["points-to-win"]);
				pointsPerGoal = stoi(config["points-per-goal"]);

				maxFrameTime = std::chrono::milliseconds(stoi(config["millis-per-frame"]));
				playerSpeed = stof(config["player-speed"]);
				ballSpeed = stof(config["ball-speed"]);

				ball->dim.y = stof(config["ball-radius"]);
				ball->dim.x = stof(config["ball-radius"]);

				player1->dim.x = stof(config["player-width"]);
				player1->dim.y = stof(config["player-height"]);
				player1->pos.x = -stof(config["player-offset"]);

				player2->dim.x = stof(config["player-width"]);
				player2->dim.y = stof(config["player-height"]);
				player2->pos.x = +stof(config["player-offset"]);

				top->pos.y = +stof(config["wall-offset"]);
				top->dim.x = stof(config["wall-width"]);
				top->dim.y = stof(config["wall-height"]);

				bottom->pos.y = -stof(config["wall-offset"]);
				bottom->dim.x = stof(config["wall-width"]);
				bottom->dim.y = stof(config["wall-height"]);

				// goal 1
				goal1->is_trigger = true;
				goal1->trigger_callback = [&]() {
					player2Score++;
				};
				goal1->dim.x = stof(config["goal-width"]);
				goal1->dim.y = stof(config["goal-height"]);
				goal1->pos.x = -stof(config["goal-offset"]);

				// goal 2
				goal2->is_trigger = true;
				goal2->trigger_callback = [&]() {
					player1Score++;
				};
				goal2->dim.x = stof(config["goal-width"]);
				goal2->dim.y = stof(config["goal-height"]);
				goal2->pos.x = +stof(config["goal-offset"]);

				ball->dy = ballSpeed;
				ball->dx = ballSpeed;

				// setup opengl context

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

	void run() {

		while (!glfwWindowShouldClose(glfwWindow)) {

			t1 = std::chrono::high_resolution_clock::now();

			// get input data byte from arduino
			if (port.readBytes(byte, 255)) {
				memset(byte, 0, 255);
			}

			this->update(::byteToInputData(byte[0]));

			this->draw();

			glfwSwapBuffers(glfwWindow);
			glfwPollEvents();

			t2 = std::chrono::high_resolution_clock::now();

			frameTime = t2 - t1;
			timeStep = (float)std::chrono::duration_cast<std::chrono::milliseconds>(frameTime).count() / 1000;

			//if (frameTime < maxFrameTime) {
			//	std::this_thread::sleep_for(maxFrameTime - frameTime);
			//}
		}
	}
	
	~Game() {
		delete ball;
		for (auto* aabb : aabbs) {
			delete aabb;
		}

		delete camera, renderer;

		glfwTerminate();
	}

private:

	void update(InputData data) {

		if (data.resetPressed) {
			;
		}

		// update velocities

		// player 1, the left player
		if (data.p1UpPressed) {
			player1->pos.y += playerSpeed * timeStep;
		}
		if (data.p1DownPressed) {
			player1->pos.y -= playerSpeed * timeStep;
		}

		// player 2, the right player
		if (data.p2UpPressed) {
			player2->pos.y += playerSpeed * timeStep;
		}
		if (data.p2DownPressed) {
			player2->pos.y -= playerSpeed * timeStep;
		}

		// handle collisions between ball and the environment
		for (auto* aabb : aabbs)
			ball->handleCollision(aabb);

		// update ball
		ball->pos.y += ball->dy * timeStep;
		ball->pos.x += ball->dx * timeStep;
	}

	void draw(void) {
		
		// create objects on screen:
		renderer->bufferQuad(ball->pos, ball->dim);

		for (auto* aabb : aabbs) {
			renderer->bufferQuad(aabb->pos, aabb->dim);
		}

		renderer->render(camera);

		renderer->clear();
	}

private:

	AABB* player1, * player2;
	Ball* ball;
	AABB* bottom, * top;
	AABB* goal1, * goal2;

	int player1Score, player2Score;

	std::vector<AABB*> aabbs;

	std::chrono::time_point<std::chrono::steady_clock> t1, t2;
	std::chrono::nanoseconds frameTime;

	std::chrono::milliseconds maxFrameTime;

	int height, width;
	int pointsToWin, pointsPerGoal;

	float timeStep;

	float playerSpeed;
	float ballSpeed;

	char byte[255] = {};
	SerialPort port;
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