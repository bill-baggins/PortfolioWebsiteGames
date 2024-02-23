#ifndef MAIN_GAME_LOOP_H
#define MAIN_GAME_LOOP_H

#include "game_scene.h"
#include "texture_handler.h"

class MainGameScene : public GameScene
{
public:
	MainGameScene(class EntryPoint* entry_point);
	~MainGameScene();

	void update(float dt) override;
	void draw() override;

private:
	class EntryPoint* m_entry_point;

	TextureHandler m_th;

};


#endif

