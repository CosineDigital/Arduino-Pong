#include "Game.hpp"

int main()
{
	Game game;

	game.init();
	if (!game.err) {
		game.run();
	}
	return game.err;
}


