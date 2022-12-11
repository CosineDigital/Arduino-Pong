#include <Windows.h>
#include <iostream>
#include "Serial.hpp"

int main()
{
	SerialPort port = SerialPort("COM3");

	if (!port.open) {
		return 1;
	}

	constexpr int length = 32;

	char* msg = new char[length + 1];

	if (!port.readBytes(msg, length)) {
		std::cout << msg;
	}
	else {
		std::cout << "fail";
		return 1;
	}

	return 0;
}
