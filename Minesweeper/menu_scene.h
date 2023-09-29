#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include "game_scene.h"

class MenuScene : public GameScene
{
public:
	MenuScene(class EntryPoint* entry_point);
	~MenuScene();

	void update(float dt) override;
	void draw() override;

	class EntryPoint* m_entry_point;
};

#endif

