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
	std::cout << "enter Arduino port number:\n";
	std::cin >> portNum;

	char portName[16] = { 0 };
	sprintf_s(portName, R"(\\.\COM%i)", portNum);

	SerialPort port = SerialPort(portName);

	if (!port.open) {
		std::cerr << "Port " << portName << " not open\n";
		return 0;
	}

	// Set up game:
	Game game;

	char byte;

	do {
		// get input from arduino controller:
		if (port.readByte(&byte)) {
			std::cerr << "Failed to read byte\n";
			byte = 0;
		}

		InputData data = byteToInputData(byte);

		// handle input:
		game.update(data);

		game.draw();

	} while (game.running);

	return 0;
}


