/*****************************************************************//**
 * \file   main.cpp
 * \brief  hacked this together in an evening and got it working but it needs some improvements.
 * 
 * 
 * \author willi
 * \date   May 2024
 *********************************************************************/


/*
TODO:
	1) The thread where the logic is being performed is slow, mainly due to the dead cell check.
	   find a more efficient way of calculating this.
	2) The thread that the logic runs on decides to kill it self.
*/

#define _CRT_SECURE_NO_WARNINGS

#include <map>
#include <iostream>
#include <thread>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include "raylib.h"
#include "raymath.h"
#include "mytypes.h"



constexpr f32 MIN_ZOOM = 0.10f;
constexpr f32 MAX_ZOOM = 15.0f;


static i32 SW = 800;
static i32 SH = 600;
static const char* TITLE = "HashLife";
static i32 FPS = 75;


static bool operator==(const struct Vector2& a, const struct Vector2& b)
{
	return Vector2Equals(a, b);
}

static int operator<=>(const struct Vector2& a, const struct Vector2& b)
{
	int x = b.x - a.x;
	int y = b.y - a.y;
	if (x != 0) return x;
	if (y != 0) return y;
	return 0;
}

template<>
struct std::hash<struct Vector2>
{
	std::size_t operator()(const struct Vector2& v) const
	{
		return std::hash<i32>{}(
			(i32)v.x ^ ((i32)v.y << 1)
		);
	}
};


struct Cell
{
	Cell(Vector2 pPos = Vector2{}, bool pAlive = false) : pos(pPos), alive(pAlive), nextAlive(false) {}
	~Cell() = default;

	bool operator==(const Cell& b);

	Vector2 pos;
	bool alive;
	bool nextAlive;
};

bool Cell::operator==(const Cell& b)
{
	return Vector2Equals(pos, b.pos) && alive == b.alive && nextAlive == b.nextAlive;
}


class CellGrid
{
public:
	CellGrid();
	~CellGrid();

	void Update(f32 dt);
	void Draw();

	const Camera2D& GetCamera() const;

private:
	void UpdaterThread(i32 count, i32 limit);

	void UpdateCells();
	void AddDeadCellsAround(std::map<Vector2, Cell>& t, const Cell& pos);
	bool CleanUpDeadCellsAround(const Cell& pos);

	
private:
	Camera2D m_camera;
	std::map<Vector2, Cell> m_cells;
	std::map<Vector2, Cell> m_nextCells;
	i32 m_cellWidth;
	i32 m_cellHeight;
	f32 m_scrollFactor;
	Vector2 m_clickPos;

	Texture m_aliveCellTex;
	Texture m_deadCellTex;
	bool m_paused;

	i32 m_counter;
	i32 m_counterLimit;

	std::thread m_cellUpdater;
	bool m_threadInitialized;
};


CellGrid::CellGrid() :
	m_camera(Camera2D{ .zoom = 1 }),
	m_cells({}),
	m_cellWidth(4),
	m_cellHeight(4),
	m_scrollFactor(1.001f),
	m_clickPos(Vector2{}),
	m_aliveCellTex({}),
	m_deadCellTex({}),
	m_paused(true),
	m_counter(0),
	m_counterLimit(20),
	m_cellUpdater(),
	m_threadInitialized(false)
{
	
	Image cellAliveIm = GenImageColor(m_cellWidth, m_cellHeight, GREEN);
	m_aliveCellTex = LoadTextureFromImage(cellAliveIm);
	UnloadImage(cellAliveIm);

	Image cellDeadIm = GenImageColor(m_cellWidth, m_cellHeight, BLACK);
	m_deadCellTex = LoadTextureFromImage(cellDeadIm);
	UnloadImage(cellDeadIm);
}

CellGrid::~CellGrid()
{
	m_cellUpdater.join();
	UnloadTexture(m_aliveCellTex);
	UnloadTexture(m_deadCellTex);
}

void CellGrid::Update(f32 dt)
{
	Vector2 mousePos = GetMousePosition();
	mousePos = GetScreenToWorld2D(mousePos, m_camera);

	// TODO: Putting a cell on the map when the game is not paused causes a concurrent read access violation on the
	// map. Fix this.
	if (m_paused && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		m_clickPos.x = std::floor(mousePos.x / m_cellWidth);
		m_clickPos.y = std::floor(mousePos.y / m_cellHeight);

		if (!m_cells.contains(m_clickPos) || (m_cells.contains(m_clickPos) && !m_cells.at(m_clickPos).alive))
		{
			m_cells[m_clickPos] = Cell(m_clickPos, true);
			AddDeadCellsAround(m_cells, m_cells[m_clickPos]);
		}
	}

	// TODO: Same case here. Only can clear if the game is paused.
	if (m_paused && IsKeyPressed(KEY_C))
	{
		m_cells.clear();
	}

	// for dragging the camera
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
	{
		Vector2 mouse_delta = GetMouseDelta();
		m_camera.target.x += -mouse_delta.x * (1 / m_camera.zoom);
		m_camera.target.y += -mouse_delta.y * (1 / m_camera.zoom);
	}

	// For zooming in and out.
	if (IsKeyDown(KEY_Q) || IsKeyDown(KEY_E))
	{
		if (IsKeyDown(KEY_Q))
		{
			m_camera.zoom = std::max(m_camera.zoom * 0.99f, MIN_ZOOM);
		}
		else if (IsKeyDown(KEY_E))
		{
			m_camera.zoom = std::min(m_camera.zoom * 1.01f, MAX_ZOOM);
		}

		Vector2 pos = GetScreenToWorld2D(mousePos, m_camera);
		m_camera.offset = mousePos;
		m_camera.target = Vector2{ 0, 0 };
	}

	if (IsKeyPressed(KEY_SPACE))
	{
		m_paused = !m_paused;
	}

	if (!m_threadInitialized)
	{
		m_threadInitialized = true;
		m_cellUpdater = std::thread(&CellGrid::UpdaterThread, this, m_counter, m_counterLimit);
	}
}

void CellGrid::Draw()
{
	char buffer[32] = { 0 };
	sprintf(buffer, "Position: %.0f, %.0f", m_clickPos.x, m_clickPos.y);

	// so that it draws not relative to the camera. This is shitty code though.
	EndMode2D();
	DrawText(buffer, 0, 20, 20, GOLD);
	if (m_paused)
	{
		DrawText("Paused", 0, 40, 20, GOLD);
	}
	BeginMode2D(m_camera);

	for (const auto& pair : m_cells)
	{
		i32 x = pair.second.pos.x * m_cellWidth;
		i32 y = pair.second.pos.y * m_cellHeight;

		/*Texture* tex = &m_aliveCellTex;
		if (!pair.second.alive)
		{
			tex = &m_deadCellTex;
		}*/
		
		if (pair.second.alive)
		{
			DrawTexture(m_aliveCellTex, x, y, RAYWHITE);
		}
	}
}

const Camera2D& CellGrid::GetCamera() const
{
	return m_camera;
}

void CellGrid::UpdaterThread(i32 count, i32 limit)
{
	while (1)
	{
		if (!m_paused)
		{
			count++;
			if (count >= limit)
			{
				count = 0;
				UpdateCells();
			}
		}
	}
}

void CellGrid::UpdateCells()
{
	Vector2 pos{};
	
	for (auto& pair : m_cells)
	{
		Cell& cell = pair.second;

		i32 count = 0;
		for (i32 y = -1; y < 2; y++)
		{
			for (i32 x = -1; x < 2; x++)
			{
				if (x == 0 && y == 0) continue;
				pos.x = cell.pos.x + x;
				pos.y = cell.pos.y + y;
				if (m_cells.contains(pos) && m_cells.at(pos).alive)
				{
					count++;
				}
			}
		}



		if (cell.alive)
		{
			cell.nextAlive = count == 2 || count == 3;
		}
		else
		{
			cell.nextAlive = count == 3;
		}

		if (cell.nextAlive)
		{
			AddDeadCellsAround(m_cells, cell); // possibly undefined behavior?
		}
		
	}

	for (auto& pair : m_cells)
	{
		Cell& cell = pair.second;
		cell.alive = cell.nextAlive;
		cell.nextAlive = false;
	}


	while (1)
	{
		bool shouldBreakToCheckAgain = false;
		for (auto& pair : m_cells)
		{
			shouldBreakToCheckAgain = CleanUpDeadCellsAround(pair.second);
			if (shouldBreakToCheckAgain)
			{
				break;
			}
		}

		if (!shouldBreakToCheckAgain)
		{
			break;
		}
	}
}

void CellGrid::AddDeadCellsAround(std::map<Vector2, Cell>& t, const Cell& cell)
{
	Vector2 pos{};

	for (i32 y = -2; y < 3; y++)
	{
		for (i32 x = -2; x < 3; x++)
		{
			if (x == 0 && y == 0) continue;

			pos.x = cell.pos.x + x;
			pos.y = cell.pos.y + y;

			if (!t.contains(pos))
			{
				t[pos] = Cell(pos, false);
			}
		}
	}
}


/**
 * Returns false if it did not clean up anything around this cell, true if it did.
 * 
 * \param cell The cell.
 * \return true or false
 */
bool CellGrid::CleanUpDeadCellsAround(const Cell& cell)
{
	if (cell.alive) return false;

	Vector2 pos{};
	for (i32 y = -2; y < 3; y++)
	{
		for (i32 x = -2; x < 3; x++)
		{
			pos.x = cell.pos.x + x;
			pos.y = cell.pos.y + y;
			if (m_cells.contains(pos) && m_cells[pos].alive) return false;
		}
	}

	for (i32 y = -1; y < 2; y++)
	{
		for (i32 x = -1; x < 2; x++)
		{
			pos.x = cell.pos.x + x;
			pos.y = cell.pos.y + y;
			m_cells.erase(pos);
		}
	}

	return true;
}


i32 main(i32 argc, char** argv)
{
	InitWindow(SW, SH, TITLE);
	SetTargetFPS(FPS);
	SetExitKey(KEY_NULL);

	CellGrid cellGrid;

	while (!WindowShouldClose())
	{
		f32 dt = GetFrameTime();
		
		cellGrid.Update(dt);

		BeginDrawing();
		ClearBackground(LIGHTGRAY);
		DrawFPS(0, 0);

		BeginMode2D(cellGrid.GetCamera());
		{
			cellGrid.Draw();
		}
		EndMode2D();

		EndDrawing();
	}

	CloseWindow();
}



/*if (cell.alive)
		{
			if (count == 3 || count == 2)
			{
				m_nextCells[cell.pos] = Cell(cell.pos, true);
				AddDeadCellsAround(m_nextCells, m_nextCells[cell.pos]);
			}
			else
			{
				m_nextCells[cell.pos] = Cell(cell.pos, false);
			}
		}
		else
		{
			if (count == 3)
			{
				m_nextCells[cell.pos] = Cell(cell.pos, true);
				AddDeadCellsAround(m_nextCells, m_nextCells[cell.pos]);
			}
			else
			{
				m_nextCells[cell.pos] = Cell(cell.pos, false);
			}
		}*/