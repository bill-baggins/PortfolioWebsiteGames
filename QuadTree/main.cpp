#include "raylib.h"
#include "mytypes.h"
#include <cstring>
#include <concepts>
#include <memory>

//template<typename T>
//concept CPosition = requires(T a) 
//{
//	{ a.position() } -> std::same_as<Vector2>;
//};

constexpr usize MAX_PER_NODE = 4;

enum Location 
{
	NORTHEAST,
	NORTHWEST,
	SOUTHWEST,
	SOUTHEAST,
};


class QuadTree
{
public:

	explicit QuadTree(Rectangle rect) : m_rect(rect)
	{
		memset(m_positions, 0, sizeof(Vector2) * MAX_PER_NODE);
	}

	~QuadTree()
	{

	}

	void draw()
	{
		DrawRectangleLinesEx(m_rect, 2, WHITE);
	}

private:
	Rectangle m_rect;
	Vector2 m_positions[MAX_PER_NODE];
	std::unique_ptr<QuadTree> m_nodes[MAX_PER_NODE];
};


i32 main()
{
	InitWindow(800, 600, "QuadTree");
	SetTargetFPS(60);
	SetExitKey(0);
	
	Rectangle starting_bounds = Rectangle{
		0, 0, static_cast<f32>(GetScreenWidth()), static_cast<f32>(GetScreenHeight()),
	};

	std::unique_ptr<QuadTree> tree = std::make_unique<QuadTree>(starting_bounds);
	while (!WindowShouldClose()) 
	{
		BeginDrawing();
		ClearBackground(BLACK);

		tree->draw();
		

		DrawFPS(0, 0);
		EndDrawing();
	}

	return 0;
}
