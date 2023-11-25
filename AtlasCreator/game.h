#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "mytypes.h"
#include "globals.h"

#include "selector.h"
#include "menu.h"

typedef struct Game
{
	Selector selector;
	Menu menu;
} Game;

Game* Game_new(void);
void Game_run(Game* g);

#endif