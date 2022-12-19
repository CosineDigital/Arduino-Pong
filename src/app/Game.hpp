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

#include "../input/input_data.hpp"
#include "../graphics/renderer.hpp"
#include "../core/Ball.hpp"
#include "../input/serial_port.hpp"
#include "../input/config.hpp"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, int button, int action, int bits);
static void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int bits);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void cursor_callback(GLFWwindow* window, double xpos, double ypos);
static void glfw_error_callback(int error, const char* description);

enum class WinState {
	NONE,
	P1Win,
	P2Win
};

class Game final {

public:
	Game() {
		port = new SerialPort();

		ball = new Ball();

		player1 = new Rect();
		player2 = new Rect();

		top = new Rect();
		bottom = new Rect();

		background = new Rect();

		rects = { player1, player2, top, bottom };
	}

	int init() {

		Config config;
		config.ignore(';');

		if (config.load("resources/config.txt")) {

			// open port
			// Set up the port for comminicating with the arduino
			char portName[16] = { 0 };
			sprintf_s(portName, "\\\\.\\COM%i", config.getInt("arduino-port"));

			if (port->init(portName)) {

				// set up the game
				width = config.getInt("game-dim-x");
				height = config.getInt("game-dim-y");
				pointsToWin = config.getInt("points-to-win");
				pointsPerGoal = config.getInt("points-per-goal");

				maxFrameTime = std::chrono::milliseconds(config.getInt("millis-per-frame"));
				playerSpeed = config.getFloat("player-speed");
				ballSpeed = config.getFloat("ball-speed");
				ballSpeedMultiplier = config.getFloat("ball-speed-multiplier");

				background->pos = { 0, 0 };
				background->dim = { 1, 1 };

				ball->dim.y = config.getFloat("ball-radius");
				ball->dim.x = config.getFloat("ball-radius");

				ballColor = config.getInt("ball-color");

				player1->dim.x = config.getFloat("player-dim-x");
				player1->dim.y = config.getFloat("player-dim-y");
				player1->pos.x = -config.getFloat("player-offset-x");
				player1->is_player = true;
				P1Color = config.getInt("player-1-color");

				player2->dim.x = config.getFloat("player-dim-x");
				player2->dim.y = config.getFloat("player-dim-y");
				player2->pos.x = +config.getFloat("player-offset-x");
				player2->is_player = true;
				P2Color = config.getInt("player-2-color");

				// set up walls
				top->pos.y = +config.getFloat("wall-offset-y");
				top->dim.x = config.getFloat("wall-dim-x");
				top->dim.y = config.getFloat("wall-dim-y");

				bottom->pos.y = -config.getFloat("wall-offset-y");
				bottom->dim.x = config.getFloat("wall-dim-x");
				bottom->dim.y = config.getFloat("wall-dim-y");

				bool spawnOtherWalls = config.getInt("spawn-other-walls");
				float otherWallOffsetX = config.getFloat("other-wall-offset-x");
				float otherWallOffsetY = config.getFloat("other-wall-offset-y");
				float otherWallWidth = config.getFloat("other-wall-dim-x");
				float otherWallHeight = config.getFloat("other-wall-dim-y");
				if (spawnOtherWalls) {
					// spawn four walls in the corners of the game
					for (int i = -1; i < 2; i += 2) {
						for (int j = -1; j < 2; j += 2) {
							Rect* otherWall = new Rect();

							otherWall->dim = { otherWallWidth, otherWallHeight };
							otherWall->pos = { otherWallOffsetX * i, otherWallOffsetY * j};
							rects.push_back(otherWall);
						}
					}
				}

				wallColor = config.getInt("wall-color");

				// pause time between goals and after reset
				resetPauseTime = config.getFloat("reset-pause-time");
				// by default have a pause before starting the game
				resetPauseTimer = resetPauseTime;

				winstate = WinState::NONE;

				Rect* goal1 = new Rect();
				Rect* goal2 = new Rect();

				// goal 1
				goal1->is_trigger = true;
				goal1->trigger_callback = [&]() {
					player2Points += pointsPerGoal;
					onGoal();
				};
				goal1->dim.x = config.getFloat("goal-dim-x");
				goal1->dim.y = config.getFloat("goal-dim-y");
				goal1->pos.x = -config.getFloat("goal-offset-x");

				// goal 2
				goal2->is_trigger = true;
				goal2->trigger_callback = [&]() {
					player1Points += pointsPerGoal;
					onGoal();
				};
				goal2->dim.x = config.getFloat("goal-dim-x");
				goal2->dim.y = config.getFloat("goal-dim-y");
				goal2->pos.x = +config.getFloat("goal-offset-x");

				rects.push_back(goal1);
				rects.push_back(goal2);

				// set up the text
				float textOffsetX = config.getFloat("text-offset-x");
				float textOffsetY = config.getFloat("text-offset-y");
				float textWidth = config.getFloat("text-dim-x");
				float textHeight = config.getFloat("text-dim-y");
				// left -> right, first left
				for (int i = -1; i < 2; i += 2) {
					for (int j = -1; j < 2; j += 2) {
						Rect* text = new Rect();
							
						float middle = i * textOffsetX - textWidth;

						text->dim = { textWidth, textHeight };
						text->pos = {
							middle + j * textWidth,
							textOffsetY
						};

						texts.push_back(text);
					}
				}


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
			std::cerr << "Could not load config file\n";
			return 0;
		}
	}

	static constexpr unsigned char REQUEST_READY = 1 << 7;
	static constexpr unsigned char REQUEST_HOLD  = 1 << 6;

	void run() {

		Reset();

		while (!glfwWindowShouldClose(glfwWindow)) {

			t1 = std::chrono::high_resolution_clock::now();

			// only update the game if the game is not paused
			if (resetPauseTimer < 0) {

				// reset winstate after pause timer has ended
				winstate = WinState::NONE;

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
		for (auto* aabb : rects) {
			delete aabb;
		}

		delete background;

		for (auto* text : texts) {
			delete text;
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

			// don't do anything else
			return;
		}


		// handle collisions between ball and the environment
		for (auto* aabb : rects) {

			if (aabb->is_player) {

				// use aabb collision detection for players
				CollisionDirection direction = ball->checkCollisionAABB(aabb);

				if (direction == CollisionDirection::UNKNOWN) {
					// some sort of collision, use the angle

					// vector pointing towards the center of the ball
					glm::vec2 offset = ball->pos - aabb->pos;
					// [-pi, pi]
					float angle = std::atan2f(offset.y, offset.x);

					float l = std::sqrt(ball->vel.x * ball->vel.x + ball->vel.y * ball->vel.y);

					ball->vel.x = std::cosf(angle) * l;
					ball->vel.y = std::sinf(angle) * l;

					// on collision increase the speed
					ball->vel *= ballSpeedMultiplier;
				}
			}
			else {

				// use swept collision for everything else
				CollisionDirection direction = ball->checkCollisionSwept(aabb, timeStep);

				if (direction != CollisionDirection::NONE) {
					if (aabb->is_trigger) {
						aabb->trigger_callback();
						continue;
					}

					// walls reflect the ball perfectly
					switch (direction) {
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

					// on collision increase the speed
					ball->vel *= ballSpeedMultiplier;
				}
			}
		}
		
		// update ball
		ball->pos += ball->vel * timeStep;


		// player 1, the left player
		bool belowTop;
		bool aboveBottom;

		// check player 1
		aboveBottom = player1->pos.y >= bottom->pos.y + bottom->dim.y + player1->dim.y;
		belowTop = player1->pos.y <= top->pos.y - top->dim.y - player1->dim.y;

		if (aboveBottom && belowTop) {
			// move like usual
			if (data.P1Up) {
				player1->pos.y += playerSpeed * timeStep;
			}
			if (data.P1Down) {
				player1->pos.y -= playerSpeed * timeStep;
			}

		}
		else if (!aboveBottom) {
			player1->pos.y = bottom->pos.y + bottom->dim.y + player1->dim.y;
		}
		else if (!belowTop) {
			player1->pos.y = top->pos.y - top->dim.y - player1->dim.y;
		}


		// check player 2
		aboveBottom = player2->pos.y >= bottom->pos.y + bottom->dim.y + player2->dim.y;
		belowTop = player2->pos.y <= top->pos.y - top->dim.y - player2->dim.y;

		if (aboveBottom && belowTop) {
			// move like usual
			if (data.P2Up) {
				player2->pos.y += playerSpeed * timeStep;
			}
			if (data.P2Down) {
				player2->pos.y -= playerSpeed * timeStep;
			}

		}
		else if (!aboveBottom) {
			player2->pos.y = bottom->pos.y + bottom->dim.y + player2->dim.y;
		}
		else if (!belowTop) {
			player2->pos.y = top->pos.y - top->dim.y - player2->dim.y;
		}
	}

	void draw(void) {

		// draw the background, so we don't need to clear the buffer
		renderer->bufferQuad(background, 11);

		// draw scores if no one has won
		if (winstate == WinState::NONE) {
			if (player1Points > 9) {
				renderer->bufferQuad(texts[0], player1Points / 10);
			}
			renderer->bufferQuad(texts[1], player1Points % 10);

			if (player2Points > 9) {
				renderer->bufferQuad(texts[2], player2Points / 10);
			}
			renderer->bufferQuad(texts[3], player2Points % 10);
		}
		else if (winstate == WinState::P1Win) {
			renderer->bufferQuad(texts[1], 18); // W (see renderer.cpp)
			renderer->bufferQuad(texts[3], 19); // L
		}
		else if (winstate == WinState::P2Win) {
			renderer->bufferQuad(texts[1], 19); // L
			renderer->bufferQuad(texts[3], 18); // W
		}
		


		// create objects on screen:
		renderer->bufferQuad(ball, ballColor);

		for (auto* rect : rects) {
			// don't draw goals or players
			if (rect->is_trigger || rect->is_player) {
				continue;
			}

			// draw the walls
			renderer->bufferQuad(rect, wallColor);
		}

		renderer->bufferQuad(player1, P1Color);
		renderer->bufferQuad(player2, P2Color);

		renderer->render(camera);

		renderer->clear();

		glfwSwapBuffers(glfwWindow);
		glfwPollEvents();
	}

	inline float randomUnit() noexcept {
		return randomEngine(gen);
	}

	// @return an angle in radians [0, pi]
	inline float randomAngle() noexcept {
		return randomUnit() * 3.1415926535897932384626433832795f;
	}

	void onGoal() noexcept {
		if (player1Points >= pointsToWin) {
			winstate = WinState::P1Win;
			player1Points = player2Points = 0;
		}
		else if (player2Points >= pointsToWin) {
			winstate = WinState::P2Win;
			player1Points = player2Points = 0;
		}
		Reset();
	}

	void Reset() {
		// reset the game, wait then place the ball again
		ball->pos = glm::vec2{};

		// ball can only fly off in certain directions
		float multiplier = randomUnit() - 0.5f;

		float angle = randomAngle() * multiplier;
		ball->vel = { ballSpeed * std::cosf(angle), ballSpeed * std::sinf(angle) };

		player1->pos.y = player2->pos.y = 0;

		timeStep = 0;
		resetPauseTimer = resetPauseTime;
	}

private:

	std::random_device rd;
	std::mt19937 gen = std::mt19937(rd());
	std::uniform_real_distribution<float> randomEngine = std::uniform_real_distribution<float>(0.f, 1.f);

	Rect* player1, * player2;
	Ball* ball;
	Rect* bottom, * top;
	Rect* background;

	WinState winstate;

	int P1Color, P2Color, ballColor, wallColor;

	int player1Points, player2Points;

	std::vector<Rect*> texts;
	std::vector<Rect*> rects;

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