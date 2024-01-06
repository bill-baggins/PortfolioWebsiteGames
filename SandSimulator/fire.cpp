#include "particle_funcs.h"
#include "world.h"
#include "globals.h"

#include "raylib.h"
#include "raymath.h"

#include <cstdio>

static bool calculate_next_move(Particle* p);
static bool can_burn(i32 x, i32 y);

static i32 CAN_BURN_BITS = WOOD;
static i32 MOVE_OPS_FIRE[][2] = {
	{ -1, 0 },
	{ 0, -1 },
	{ 1,  0},
	{ 0,  1},
	{-1, -1},
	{-1, 1},
	{1, 1},
	{1, -1},
};

void init_fire(Particle* p)
{
	p->type = FIRE;
	p->index = I_FIRE;
	p->color = FIRE_COLOR;
	p->color = Color{
		p->color.r,
		p->color.g,
		0,
		150
	};
}

void update_draw_fire(Particle* p, f32 dt)
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

	i32 air_count = 0;

	for (i32 i = 0; i < 8; i++) 
	{
		i32 x = sx + MOVE_OPS_FIRE[i][0];
		i32 y = sy + MOVE_OPS_FIRE[i][1];
		i32 coord = y * MAX_WIDTH + x;

		if (is_inbounds(x, y) && grid_arr[coord].type & AIR)
		{
			air_count += 1;
			if (air_count == 4) break;
		}
	}

	for (i32 i = 0; i < 8; i++)
	{
		i32 x = sx + MOVE_OPS_FIRE[i][0];
		i32 y = sy + MOVE_OPS_FIRE[i][1];
		i32 coord = y * MAX_WIDTH + x;
		f32 rand_threshold = (f32)1 / (f32)GetRandomValue(0, 64);

		if (!is_inbounds(x, y))
		{
			continue;
		}

		if (rand_threshold > 0.22f && can_burn(x, y))
		{
			init_func[I_FIRE](&grid_arr[coord]);
			p->next_pos = Vector2{ (f32)x, (f32)y };
			return false;
		}
	}

	if (air_count == 4)
	{
		init_func[I_SMOKE](p);
	}

	return false;
}

static bool can_burn(i32 x, i32 y)
{
	i32 coord = y * MAX_WIDTH + x;
	return grid_arr[coord].type & CAN_BURN_BITS;
}
