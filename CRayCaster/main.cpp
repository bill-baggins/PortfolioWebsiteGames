#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cmath>
#include <vector>

#include "raylib.h"
#include "mytypes.h"
#include "logging.h"

#include "imgui.h"
#include "rlImGui.h"

constexpr int SW = 1200;
constexpr int SH = 720;
constexpr char TITLE[] = "cRayCaster";

enum CellType
{
	EMPTY,
	WALL
};

struct Cell
{
	explicit Cell(f32 cell_width, f32 cell_height);
	explicit Cell(f32 cell_width, f32 cell_height, CellType type);
	explicit Cell(CellType type, Rectangle rect);

	~Cell()  = default;
	
	bool operator==(const Cell& other);

	CellType m_type;
	Rectangle m_rect;
};

Cell::Cell(f32 cell_width, f32 cell_height) :
	m_type(EMPTY),
	m_rect(Rectangle{0, 0, cell_width, cell_height})
{
}

Cell::Cell(f32 cell_width, f32 cell_height, CellType type) :
	m_type(type),
	m_rect(Rectangle{0, 0, cell_width, cell_height})
{
}

Cell::Cell(CellType type, Rectangle rect) :
	m_type(type),
	m_rect(rect)
{
}


bool Cell::operator==(const Cell& other)
{
	return m_type == other.m_type && m_rect.x == other.m_rect.x &&
		m_rect.y == other.m_rect.y && m_rect.width == other.m_rect.width &&
		m_rect.height == other.m_rect.height;
}

class Grid 
{
public:
	Grid(u32 width, u32 height, u32 cell_width, u32 cell_height);
	~Grid();

	void update(f32 dt);
	void draw();
private:

private:
	std::vector<Cell> m_world;
	u32 m_width;
	u32 m_height;
	u32 m_cell_width;
	u32 m_cell_height;

	Texture m_cell_tex;

	RenderTexture2D m_render_tex;
	Vector2 m_render_tex_pos;
	Rectangle m_render_tex_src_rect;
};


Grid::Grid(u32 width, u32 height, u32 cell_width, u32 cell_height) :
	m_width(width),
	m_height(height),
	m_cell_width(cell_width),
	m_cell_height(cell_height),
	m_cell_tex({}),
	m_render_tex({}),
	m_render_tex_pos({}),
	m_render_tex_src_rect({})
{
	Image im = GenImageColor(cell_width, cell_height, BLUE);
	m_cell_tex = LoadTextureFromImage(im);
	UnloadImage(im);

	m_render_tex = LoadRenderTexture(width*cell_width, height*cell_height);
	m_render_tex_pos = Vector2{ 
		(f32)GetScreenWidth() / 2 - (width*cell_width) / 2, 
		(f32)GetScreenHeight() / 2 - (height*cell_height) / 2,
	};

	m_render_tex_src_rect = Rectangle{ 0, 0, (f32)m_render_tex.texture.width, (f32)(-m_render_tex.texture.height) };

	m_world = std::vector<Cell>(width*height, Cell(m_cell_width, m_cell_height));

	for (u32 i = 0; i < width; i++)
	{
		m_world[i] = Cell(WALL, Rectangle{(f32)i, 0, (f32)m_cell_width, (f32)m_cell_height});
		m_world[(height - 1) * width + i] = Cell(WALL, Rectangle{ (f32)i, (f32)height-1, (f32)m_cell_width, (f32)m_cell_height});
	}

	for (u32 i = 0; i < height; i++)
	{
		m_world[i * width] = Cell(WALL, Rectangle{ 0, (f32)i, (f32)m_cell_width, (f32)m_cell_height});
		m_world[i * width + (width - 1)] = Cell(WALL, Rectangle{ (f32)width - 1, (f32)i, (f32)m_cell_width, (f32)m_cell_height });
	}
}

void Grid::update(f32 dt)
{
	Vector2 pos = GetMousePosition();
	bool oob = false;
	pos.x -= m_render_tex_pos.x;
	pos.y -= m_render_tex_pos.y;
	pos.x = floorf(pos.x / m_cell_width);
	pos.y = floorf(pos.y / m_cell_height);

	if ((u32)pos.x >= m_width - 1 || (u32)pos.x <= 0 || (u32)pos.y <= 0 || (u32)pos.y >= m_height - 1)
	{
		oob = true;
	}

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !oob)
	{
		m_world[(u32)pos.y * m_width + (u32)pos.x] = Cell(WALL, Rectangle{ pos.x, pos.y, (f32)m_cell_width, (f32)m_cell_height });
	}
	
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && !oob)
	{
		m_world[(u32)pos.y * m_width + (u32)pos.x] = Cell(EMPTY, Rectangle{ pos.x, pos.y, (f32)m_cell_width, (f32)m_cell_height });;
	}

	if (IsKeyPressed(KEY_E))
	{
		for (u32 y = 1; y < m_height - 1; y++)
		{
			for (u32 x = 1; x < m_width - 1; x++)
			{
				m_world[y * m_width + x] = Cell(WALL, Rectangle{ (f32)x, (f32)y, (f32)m_cell_width, (f32)m_cell_height });;
			}
		}
	}
}

void Grid::draw()
{
	BeginTextureMode(m_render_tex);
	
	ClearBackground(BLACK);

	for (u32 i = 0; i < m_width * m_height; i++)
	{
		if (m_world[i].m_type == EMPTY)
		{
			continue;
		}

		u32 y = i / m_width;
		u32 x = i % m_width;
		DrawTexture(m_cell_tex, (i32)(x * m_cell_width), (i32)(y * m_cell_height), WHITE);
	}

	EndTextureMode();

	DrawTextureRec(
		m_render_tex.texture,
		m_render_tex_src_rect,
		m_render_tex_pos,
		WHITE
	);
}

Grid::~Grid()
{
	UnloadTexture(m_cell_tex);
}


i32 main(i32 argc, mstr_t* args)
{
	InitWindow(SW, SH, TITLE);
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
	SetExitKey(KEY_NULL);

	Grid grid(40, 30, 20, 20);

	while (!WindowShouldClose())
	{
		f32 dt = GetFrameTime();

		grid.update(dt);
		
		BeginDrawing();

		ClearBackground(LIGHTGRAY);
		DrawFPS(0, 0);

		grid.draw();

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
