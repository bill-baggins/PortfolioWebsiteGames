#include "particle_funcs.h"
#include "world.h"
#include "globals.h"

#include "raylib.h"
#include "raymath.h"

#include <cstdio>

static bool calculate_next_move(Particle* p);
static bool is_blocking(i32 x, i32 y);

static i32 BLOCKING_BITS_SAND = SAND | STONE | WOOD;
static i32 MOVE_OPS_SAND[][2] = {
	{ 0, 1 },
	{ 1, 1 },
	{-1, 1 },
};

static i32 RAND_POOL_SAND[3][2] = { 0 };

void init_sand(Particle* p)
{
	p->type = SAND;
	p->index = I_SAND;
	p->color = SAND_COLOR;
	p->color = Color{
		p->color.r,
		(u8)(p->color.g + p->offset.g),
		(u8)(p->color.b + p->offset.b),
		255
	};
	p->health = 20;
}

void update_draw_sand(Particle* p, f32 dt)
{
	p->vel.y += std::min(GRAVITY * dt, TERM_VEL);
	bool has_stopped = calculate_next_move(p);
	if (has_stopped)
	{
		p->vel = Vector2{};
	}
	Vector2 draw_pos = Vector2{ p->pos.x * PIXEL_WIDTH, p->pos.y * PIXEL_HEIGHT };
	DrawTextureV(*p->texture, draw_pos, p->color);
}

static bool calculate_next_move(Particle* p)
{
	i32 sx = p->pos.x;
	i32 sy = p->pos.y + p->vel.y;

	// Vertically out of bounds, do not consider.
	if (!is_inbounds(0, sy))
	{
		return true;
	}

	i32 push_ind = 0;

	for (i32 i = 0; i < 3; i++)
	{
		i32 x = sx + MOVE_OPS_SAND[i][0];
		i32 y = sy + MOVE_OPS_SAND[i][1];
		if (!is_inbounds(x, y))
		{
			continue;
		}

		if (!is_blocking(x, y))
		{
			RAND_POOL_SAND[push_ind][0] = x;
			RAND_POOL_SAND[push_ind][1] = y;
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

	i32 *rand_next_pos = RAND_POOL_SAND[GetRandomValue(0, push_ind - 1)];
	p->next_pos = Vector2{ (f32)rand_next_pos[0], (f32)rand_next_pos[1] };

	return false;
}

static bool is_blocking(i32 x, i32 y)
{
	i32 coord = y * MAX_WIDTH + x;
	return grid_arr[coord].type & BLOCKING_BITS_SAND;
}
