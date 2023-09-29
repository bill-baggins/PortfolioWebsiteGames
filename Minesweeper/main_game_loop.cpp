#include "main_game_loop.h"

#include "raylib.h"
#include "game_scene.h"
#include "entry_point.h"
#include "menu_scene.h"

#include <iostream>

MainGameLoop::MainGameLoop(EntryPoint* entry_point)
{
	m_entry_point = entry_point;
}

MainGameLoop::~MainGameLoop()
{
	std::cout << "destructed the main game loop classs \n";
}

void MainGameLoop::update(float dt)
{
	if (IsKeyPressed(KEY_ESCAPE))
	{
		// switches the scene.
		m_entry_point->m_current_scene = std::make_unique<MenuScene>(m_entry_point);
		return;
	}

}

void MainGameLoop::draw()
{
	BeginDrawing();

	ClearBackground(SKYBLUE);

	EndDrawing();
}
