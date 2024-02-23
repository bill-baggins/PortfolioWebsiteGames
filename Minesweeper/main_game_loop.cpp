#include "main_game_loop.h"

#include "raylib.h"
#include "game_scene.h"
#include "entry_point.h"
#include "menu_scene.h"

#include <iostream>

MainGameScene::MainGameScene(EntryPoint* entry_point)
{
	m_entry_point = entry_point;
	m_th = TextureHandler(1.f);
}

MainGameScene::~MainGameScene()
{
	std::cout << "destructed the main game loop classs \n";
}

void MainGameScene::update(float dt)
{
	if (IsKeyPressed(KEY_ESCAPE))
	{
		// switches the scene.
		m_entry_point->m_current_scene = std::make_unique<MenuScene>(m_entry_point);
		return;
	}

}

void MainGameScene::draw()
{
	BeginDrawing();

	ClearBackground(SKYBLUE);
	


	EndDrawing();
}
