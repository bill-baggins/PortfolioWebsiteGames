#ifndef GAME_SCENE_H
#define GAME_SCENE_H

class GameScene
{
public:
	virtual ~GameScene() = 0;

	virtual void update(float dt) = 0;
	virtual void draw() = 0;
};

inline GameScene::~GameScene() {}

#endif
