#ifndef GAME_H
#define GAME_H

#include "raylib.h"

class Game
{
public:
	Game();
	~Game();
	void Run();

private:
	void Input(float dt);
	void Update(float dt);
	void Draw();

	void ControlCamera(float dt);

private:
	Camera2D m_camera;
};

#endif
