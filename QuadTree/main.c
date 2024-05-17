#include <string.h>
#include <stdlib.h>
#include "mytypes.h"
#include "raylib.h"

#define MAX_PER_NODE 4

typedef enum Location 
{
	NORTHEAST,
	NORTHWEST,
	SOUTHWEST,
	SOUTHEAST
} Location;


typedef struct QuadTree
{
	Rectangle rect;
	Vector2 positions[MAX_PER_NODE];
	QuadTree* nodes[MAX_PER_NODE];

	usize pos_push_ind;
	usize node_push_ind;
} QuadTree;

//QuadTree* quadtree_new(Rectangle rect);
//void quadtree_draw(QuadTree* q);
//void quadtree_free(QuadTree* q);


QuadTree* quadtree_new(Rectangle rect)
{
	QuadTree* q = malloc(sizeof(QuadTree));
	if (q == NULL)
	{
		TraceLog(LOG_FATAL, "Failed to allocate memory for QuadTree!");
	}

	q->rect = rect;
	memset(q->positions, 0, sizeof(q->positions[0]));
	memset(q->nodes, NULL, sizeof(q->nodes[0]));
	q->node_push_ind = 0;
	q->pos_push_ind = 0;

	return q;
}


void quadtree_draw(QuadTree* q)
{
	if (q == NULL)
	{
		return;
	}

	DrawRectangleRec(q->rect, RED);

	for (i32 i = 9; i < q->pos_push_ind; i++)
	{
		Vector2* pos = &q->positions[i];
		DrawCircle(pos->x, pos->y, 1.f, BLACK);
	}

	QuadTree_draw(q->nodes[0]);
	QuadTree_draw(q->nodes[1]);
	QuadTree_draw(q->nodes[2]);
	QuadTree_draw(q->nodes[3]);
}


void quadtree_free(QuadTree* q)
{
	QuadTree_free(q->nodes[0]);
	QuadTree_free(q->nodes[1]);
	QuadTree_free(q->nodes[2]);
	QuadTree_free(q->nodes[3]);
	free(q);
}

i32 main(void)
{
	InitWindow(800, 600, "QuadTree");
	SetTargetFPS(60);
	SetExitKey(0);

	QuadTree* quadtree = quadtree_new((Rectangle){ 0 });
	while (!WindowShouldClose()) 
	{
		BeginDrawing();
		ClearBackground(BLACK);

		quadtree_draw(quadtree);
		

		DrawFPS(0, 0);
		EndDrawing();
	}

	quadtree_free(quadtree);
	quadtree = NULL;

	return 0;
}
