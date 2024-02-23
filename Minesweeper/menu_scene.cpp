#include "menu_scene.h"
#include "main_game_loop.h"
#include "entry_point.h"
#include "raylib.h"

#include <iostream>

MenuScene::MenuScene(EntryPoint *entry_point)
{
	m_entry_point = entry_point;
}

MenuScene::~MenuScene()
{
	std::cout << "destructed the menu scnene.\n";
}

void MenuScene::update(float dt)
{
	if (IsKeyPressed(KEY_ESCAPE))
	{
		m_entry_point->m_current_scene = std::make_unique<MainGameScene>(m_entry_point);
		return;
	}
}

void MenuScene::draw()
{
	BeginDrawing();

	ClearBackground(DARKBLUE);
	
	
	EndDrawing();
}
