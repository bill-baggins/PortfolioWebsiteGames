#include "game.h"
#include "options.h"

Game::Game()
{
	InitWindow(Options::Width, Options::Height, Options::Title);
	SetTargetFPS(Options::FPS);
	SetExitKey(0);

	m_camera = {};
	m_camera.zoom = 1;
}

Game::~Game()
{
	CloseWindow();
}

void Game::Input(float dt)
{

}

void Game::Update(float dt)
{
	ControlCamera(dt);
}

void Game::Draw()
{

	BeginDrawing();
	
	ClearBackground(SKYBLUE);
	DrawFPS(0, 0);
	BeginMode2D(m_camera);
	{
		DrawCircle(Options::Width / 2, Options::Height / 2, 5, RED);
	}
	EndMode2D();
	
	EndDrawing();
}

void Game::Run()
{
	while (!WindowShouldClose())
	{
		float dt = GetFrameTime();
		Input(dt);
		Update(dt);
		Draw();
	}
}

void Game::ControlCamera(float dt)
{
	if (IsKeyDown(KEY_W))
	{
		m_camera.target.y -= 200 * dt * (1 / m_camera.zoom);
	}
	else if (IsKeyDown(KEY_S))
	{
		m_camera.target.y += 200 * dt * (1 / m_camera.zoom);
	}

	if (IsKeyDown(KEY_A))
	{
		m_camera.target.x -= 200 * dt * (1 / m_camera.zoom);
	}
	else if (IsKeyDown(KEY_D))
	{
		m_camera.target.x += 200 * dt * (1 / m_camera.zoom);
	}
}