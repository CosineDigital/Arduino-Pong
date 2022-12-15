#include "Game.hpp"

int main()
{
	/*
	char buff[255] = {0};
	memset(buff, 0, 255);

	SerialPort p;
	p.init("\\\\.\\COM3");

	while (true) {
		p.readBytes(buff, 255);

		std::cout << buff;
	}
	*/

	Game game;

	if (game.init())
		game.run();

	return 0;
}


