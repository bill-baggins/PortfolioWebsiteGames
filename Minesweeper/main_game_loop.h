#ifndef MAIN_GAME_LOOP_H
#define MAIN_GAME_LOOP_H

#include "game_scene.h"

class MainGameLoop : public GameScene
{
public:
	MainGameLoop(class EntryPoint* entry_point);
	~MainGameLoop();

	void update(float dt) override;
	void draw() override;

private:
	class EntryPoint* m_entry_point;
};


#endif

