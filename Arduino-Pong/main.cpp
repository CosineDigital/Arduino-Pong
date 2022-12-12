#include <Windows.h>
#include <iostream>
#include <cstdio>
#include <chrono>

#include "Serial.hpp"
#include "InputData.hpp"
#include "Game.hpp"

int main()
{
	int portNum;
	std::cout << "Enter Arduino port number:\n";
	std::cin >> portNum;

	char portName[16] = { 0 };
	sprintf_s(portName, R"(\\.\COM%i)", portNum);

	SerialPort port = SerialPort(portName);

	if (!port.open) {
		std::cerr << "Port " << portName << " is not open\n";
		return 0;
	}

	// Set up game:
	Game game;

	char byte = { 0 };

	do {
		// get input from arduino controller:
		if (port.readByte(&byte)) {
			std::cerr << "Failed to read byte\n";
			byte = 0;
		}

		InputData data = byteToInputData(byte);

		// run game:
		game.update(data);

	} while (game.running);

	return 0;
}


