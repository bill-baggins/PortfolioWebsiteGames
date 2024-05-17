// Working implementation of getting more realistic water physics.
// Still slightly limited, but is a good middle ground from where it was before.
// Main issue is performance, and the code for doing collision checks
// is a naive implementation. Performace for the function will worsen if the
// gravity constant is increased.
// The issue might partly go away if I update the world using multiple threads and
// chunks, but still important to note.
// This performance issue also exists for acid.

// Rishi suggested an "energy" system where each particle will start with a potential energy
// (a "score") which will increase for every successful spot downwards it travels. This score can
// be used to determine the number of spots to look at for possible movement options (instead of using
// velocity). 

#include "particle_funcs.h"
#include "world.h"
#include "globals.h"
#include "logging.h"

#include "raymath.h"

#include <stdio.h>


static void calculate_next_move(Particle* p, f32 dt);
static bool is_blocking(i32 x, i32 y);

static i32 WATER_EXCLUSION_BITS = (AIR | TOXIC_GAS | STEAM | SMOKE | ACID);

static i32 MOVE_OPS_WATER[][2] = {
	{ 0, 1},
	{ -1, 1 },
	{ 1, 1},
	{ 1, 0 },
	{ -1, 0 },
};

void init_water(Particle* p)
{
	p->type = WATER;
	p->index = I_WATER;
	p->vel = Vector2{};
	p->color = WATER_COLOR;
	p->color = Color {
		p->color.r,
		(u8)(p->color.g + p->offset.g),
		(u8)(p->color.b + p->offset.b),
		200
	};
}

void update_water(Particle* p, f32 dt)
{
	p->vel.y += std::min(GRAVITY * dt, TERM_VEL);
	calculate_next_move(p, dt);
}

void draw_water(Particle* p, f32 dt)
{
	Vector2 draw_pos = Vector2{ p->pos.x * PIXEL_WIDTH, p->pos.y * PIXEL_HEIGHT };
	DrawTextureV(*p->texture, draw_pos, p->color);
}

static void calculate_next_move(Particle* p, f32 dt)
{
	i32 sx = p->pos.x;
	i32 sy = p->pos.y;

	p->next_pos = p->pos;

	for (i32 i = 0; i < 5; i++)
	{
		bool reached_end_of_loop = false;
		i32 y_limit = ceilf(p->vel.y) + 1;
		for (i32 j = 0; j < y_limit; j++)
		{
			i32 dx = sx + MOVE_OPS_WATER[i][0] * j;
			i32 dy = sy + MOVE_OPS_WATER[i][1] * j;
			i32 coord = dy * MAX_WIDTH + dx;
			if (j + 1 == y_limit)
			{
				reached_end_of_loop = true;
			}

			if (is_inbounds(dx, dy) && !is_blocking(dx, dy))
			{
				p->next_pos = Vector2{ (f32)dx, (f32)dy };
			}
		}

		if (reached_end_of_loop && !Vector2Equals(p->next_pos, p->pos))
		{
			return;
		}
	}

	if (p->next_pos.y - p->pos.y < 0.0001)
	{
		p->vel.y /= 1.025f;
	}
}

static bool is_blocking(i32 x, i32 y)
{
	i32 coord = y * MAX_WIDTH + x;
	ParticleType p_type = grid_arr[coord].type;
	return !(p_type & WATER_EXCLUSION_BITS);
}


/* Old implementation of water (still used for lava though) */
//#include "particle_funcs.h"
//#include "world.h"
//#include "globals.h"
//
//#include <stdio.h>
//
//static bool calculate_next_move(Particle* p);
//static bool is_blocking(i32 x, i32 y);
//
//static i32 WATER_EXCLUSION_BITS = AIR | TOXIC_GAS | STEAM | SMOKE | LAVA;
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
//	p->color = BLUE;
//	p->color = Color{
//		p->color.r,
//		(u8)(p->color.g + p->offset.g),
//		(u8)(p->color.b + p->offset.b),
//		200
//	};
//	p->health = 10;
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
//	i32 sy = p->pos.y + p->vel.y;
//
//	// Vertically out of bounds, do not consider.
//	if (!is_inbounds(0, sy))
//	{
//		return true;
//	}
//
//	i32 push_ind = 0;
//
//	for (i32 i = 0; i < 5; i++)
//	{
//		i32 x = sx + MOVE_OPS_WATER[i][0];
//		i32 y = sy + MOVE_OPS_WATER[i][1];
//
//		if (!is_inbounds(x, y))
//		{
//			continue;
//		}
//
//		if (i == 0 && !is_blocking(x, y))
//		{
//			p->next_pos = Vector2{ (f32)x, (f32)y };
//			return false;
//		}
//
//		if (!is_blocking(x, y))
//		{
//			RAND_POOL_WATER[push_ind][0] = x;
//			RAND_POOL_WATER[push_ind][1] = y;
//			push_ind++;
//		}
//	}
//
//	// means that no spot was found, just set the next pos to
//	// what it is.
//	if (push_ind == 0)
//	{
//		p->next_pos = p->pos;
//		return true;
//	}
//
//	i32* rand_next_pos = RAND_POOL_WATER[GetRandomValue(0, push_ind - 1)];
//	p->next_pos = Vector2{ (f32)rand_next_pos[0], (f32)rand_next_pos[1] };
//
//	return false;
//}
//
//static bool is_blocking(i32 x, i32 y)
//{
//	i32 coord = y * MAX_WIDTH + x;
//	return !(grid_arr[coord].type & WATER_EXCLUSION_BITS);
//}
