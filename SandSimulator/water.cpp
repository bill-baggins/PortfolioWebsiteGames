#include "particle_funcs.h"
#include "world.h"
#include "globals.h"

#include <stdio.h>

static bool calculate_next_move(Particle* p);
static bool is_blocking(i32 x, i32 y);

static i32 WATER_EXCLUSION_BITS = AIR | TOXIC_GAS | STEAM | SMOKE | LAVA;

static i32 MOVE_OPS_WATER[][2] = {
	{ 0, 1},
	{ -1, 1 },
	{ 1, 1},
	{ 1, 0 },
	{ -1, 0 },
};
static i32 RAND_POOL_WATER[5][2] = { 0 };

void init_water(Particle* p)
{
	p->type = WATER;
	p->index = I_WATER;
	p->color = BLUE;
	p->color = Color{
		p->color.r,
		(u8)(p->color.g + p->offset.g),
		(u8)(p->color.b + p->offset.b),
		200
	};
	p->health = 10;
}

void update_draw_water(Particle* p, f32 dt)
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

	for (i32 i = 0; i < 5; i++)
	{
		i32 x = sx + MOVE_OPS_WATER[i][0];
		i32 y = sy + MOVE_OPS_WATER[i][1];

		if (!is_inbounds(x, y))
		{
			continue;
		}

		if (i == 0 && !is_blocking(x, y))
		{
			p->next_pos = Vector2{ (f32)x, (f32)y };
			return false;
		}

		if (!is_blocking(x, y))
		{
			RAND_POOL_WATER[push_ind][0] = x;
			RAND_POOL_WATER[push_ind][1] = y;
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

	i32* rand_next_pos = RAND_POOL_WATER[GetRandomValue(0, push_ind - 1)];
	p->next_pos = Vector2{ (f32)rand_next_pos[0], (f32)rand_next_pos[1] };

	return false;
}

static bool is_blocking(i32 x, i32 y)
{
	i32 coord = y * MAX_WIDTH + x;
	return !(grid_arr[coord].type & WATER_EXCLUSION_BITS);
}



// Semi failed implementation of getting more realistic water physics.

//#include "particle_funcs.h"
//#include "world.h"
//#include "globals.h"
//
//#include "raymath.h"
//
//#include <stdio.h>
//
//static bool calculate_next_move(Particle* p);
//static bool is_blocking(i32 x, i32 y);
//
//static i32 MOVE_OPS_WATER[][2] = {
//	{ 0, 1},
//	{ -1, 1 },
//	{ 1, 1},
//	{ 1, 0 },
//	{ -1, 0 },
//};
//static i32 RAND_POOL_WATER[5][2] = { 0 };
//
//void init_water(Particle* p)
//{
//	p->type = WATER;
//	p->index = I_WATER;
//	p->vel = Vector2{};
//	p->color = WATER_COLOR;
//	p->color = Color{
//		p->color.r,
//		(u8)(p->color.g + p->offset.g),
//		(u8)(p->color.b + p->offset.b),
//		200
//	};
//}
//
//void update_draw_water(Particle* p, f32 dt)
//{
//	p->vel.y += std::min(GRAVITY * dt, TERM_VEL);
//	bool has_stopped = calculate_next_move(p);
//	if (has_stopped)
//	{
//		p->vel = Vector2{};
//	}
//	Vector2 draw_pos = Vector2{ p->pos.x * PIXEL_WIDTH, p->pos.y * PIXEL_HEIGHT };
//	DrawTextureV(*p->texture, draw_pos, p->color);
//}
//
//static bool calculate_next_move(Particle* p)
//{
//	i32 sx = p->pos.x;
//	i32 sy = p->pos.y;
//
//	p->next_pos = p->pos;
//
//	for (i32 i = 0; i < 5; i++)
//	{
//		for (i32 j = (i32)p->vel.y; j >= 0; j--)
//		{
//			i32 dx = sx + MOVE_OPS_WATER[i][0] * j;
//			i32 dy = sy + MOVE_OPS_WATER[i][1] * j;
//
//			if (is_inbounds(dx, dy) && !is_blocking(dx, dy))
//			{
//				p->next_pos = Vector2{ (f32)dx, (f32)dy };
//				return false;
//			}
//		}
//	}
//
//	return false;
//}
//
//static bool is_blocking(i32 x, i32 y)
//{
//	i32 coord = y * MAX_WIDTH + x;
//	return !(grid_arr[coord].type & (AIR | TOXIC_GAS | STEAM | SMOKE));
//}


