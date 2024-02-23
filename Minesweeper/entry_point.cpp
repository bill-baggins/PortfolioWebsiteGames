#include "entry_point.h"
#include "raylib.h"

#include "globals.h"
#include "main_game_loop.h"


EntryPoint::EntryPoint()
{
	InitWindow(Globals::SCREEN_WIDTH, Globals::SCREEN_HEIGHT, Globals::TITLE);
	SetTargetFPS(Globals::FPS);
	SetExitKey(0);

	m_current_scene = std::make_unique<MainGameScene>(this);
}

EntryPoint::~EntryPoint()
{
	CloseWindow();
}

void EntryPoint::run()
{
	while (!WindowShouldClose())
	{
		update();
		draw();
	}
}

void EntryPoint::update()
{
	float dt = GetFrameTime();
	
	m_current_scene->update(dt);
}

void EntryPoint::draw()
{
	m_current_scene->draw();
}