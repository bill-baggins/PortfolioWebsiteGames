#include "particle_funcs.h"
#include "world.h"
#include "globals.h"

#include "raylib.h"
#include "raymath.h"

#include <cstdio>

static bool calculate_next_move(Particle* p);
static bool is_blocking(i32 x, i32 y);

static i32 BLOCKING_BITS_SMOKE = SAND | STONE | WOOD;
static i32 MOVE_OPS_SMOKE[][2] = {
	{ 0, -1 },
	{ -1, -1 },
	{ 1, -1},
	{ 1, 0 },
	{ -1, 0 },
};

static i32 RAND_POOL_SMOKE[5][2] = { 0 };

void init_smoke(Particle* p)
{
	p->type = SMOKE;
	p->index = I_SMOKE;
	p->color = SMOKE_COLOR;
	p->color = Color{
		p->color.r,
		(u8)(p->color.g + p->offset.g),
		(u8)(p->color.b + p->offset.b),
		150
	};
}

void update_draw_smoke(Particle* p, f32 dt)
{
	bool _ = calculate_next_move(p);
	Vector2 draw_pos = Vector2{ p->pos.x * PIXEL_WIDTH, p->pos.y * PIXEL_HEIGHT };
	DrawTextureV(*p->texture, draw_pos, p->color);
}

static bool calculate_next_move(Particle* p)
{
	i32 sx = p->pos.x;
	i32 sy = p->pos.y;

	// Vertically out of bounds, do not consider.
	if (!is_inbounds(0, sy))
	{
		return true;
	}

	i32 push_ind = 0;

	for (i32 i = 0; i < 5; i++)
	{
		i32 x = sx + MOVE_OPS_SMOKE[i][0];
		i32 y = sy + MOVE_OPS_SMOKE[i][1];

		if (!is_inbounds(x, y))
		{
			continue;
		}

		if (!is_blocking(x, y))
		{
			RAND_POOL_SMOKE[push_ind][0] = x;
			RAND_POOL_SMOKE[push_ind][1] = y;
			push_ind++;
		}
	}

	// means that no spot was found, just set the next pos to
	// what it is.
	if (push_ind == 0)
	{
		p->next_pos = p->pos;
		return true;
	}

	i32* rand_next_pos = RAND_POOL_SMOKE[GetRandomValue(0, push_ind - 1)];
	p->next_pos = Vector2{ (f32)rand_next_pos[0], (f32)rand_next_pos[1] };

	return false;
}

static bool is_blocking(i32 x, i32 y)
{
	i32 coord = y * MAX_WIDTH + x;
	return grid_arr[coord].type & BLOCKING_BITS_SMOKE;
}