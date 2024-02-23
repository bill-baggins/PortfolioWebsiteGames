#include "particle_funcs.h"
#include "world.h"
#include "globals.h"

#include "raylib.h"
#include "raymath.h"

#include <cstdio>

static bool calculate_next_move(Particle* p);
static bool can_burn(i32 x, i32 y);

static i32 CAN_BURN_BITS = WOOD | TOXIC_GAS | ACID;
static i32 MOVE_OPS_FIRE[][2] = {
	{ 0, -1 }, 
	{-1, -1},
	{1, -1},
	{-1, 1},
	{ -1, 0 },
	{ 1,  0},
	{ 0,  1},
	{1, 1},
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
	p->health = 0;
	p->vel = Vector2{};
}

void update_fire(Particle* p, f32 dt)
{
	bool _ = calculate_next_move(p);
}

void draw_fire(Particle* p, f32 dt)
{
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
			switch (grid_arr[coord].type)
			{
			case ACID:
				init_func[I_TOXIC_GAS](&grid_arr[coord]);
				init_func[I_FIRE](p);
				break;
			case TOXIC_GAS:
			case WOOD:
				init_func[I_FIRE](&grid_arr[coord]);
				// grid_arr[coord].type = static_cast<ParticleType>(static_cast<i32>(WOOD) | static_cast<i32>(FIRE));
				break;
			default:
				break;
			}
			p->next_pos = Vector2{ (f32)x, (f32)y };
			return false;
		}
	}

	f32 despawn_chance = (f32)1 / (f32)GetRandomValue(1, 64);
	if (despawn_chance > 0.40f)
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