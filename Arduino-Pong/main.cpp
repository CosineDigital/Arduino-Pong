#include <Windows.h>
#include <iostream>
#include "Serial.hpp"
#include <chrono>

int main()
{
	SerialPort port = SerialPort();

	if (!port.open) {
		return 1;
	}

	bool running = true;
	constexpr int length = 1;
	char* msg = new char[length + 1];

	do {

		if (!port.readBytes(msg, length)) {
			std::cout << msg << std::endl;
		}
		else {
			std::cout << "fail";
		}
		std::cout << msg << std::endl;


		
		

	} while (running);

	delete[] msg;
	return 0;
}


