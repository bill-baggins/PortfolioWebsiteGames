#include "game.h"

int main(int argc, char* argv[]) {
	struct Game* game = Game_new();
	Game_run(game);

	return 0;
}