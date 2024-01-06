#include "particle_funcs.h"
#include "world.h"
#include "globals.h"

#include <stdio.h>

static bool calculate_next_move(Particle* p);
static bool is_blocking(i32 x, i32 y);
static void handle_particle_interaction(Particle* p, i32 x, i32 y);

static i32 LAVA_EXCLUSION_BITS = AIR | SAND | WATER | ACID | WOOD | FIRE;

static i32 MOVE_OPS_LAVA[][2] = {
	{ 0, 1},
	{ -1, 1 },
	{ 1, 1},
	{ 1, 0 },
	{ -1, 0 },
};
static i32 RAND_POOL_LAVA[5][2] = { 0 };

void init_lava(Particle* p)
{
	p->type = LAVA;
	p->index = I_LAVA;
	p->color = LAVA_COLOR;
	p->color = Color{
		(u8)(p->color.r - GetRandomValue(10, 30)),
		(u8)(p->color.g - 40),
		0,
		245
	};
}

void update_draw_lava(Particle* p, f32 dt)
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
		i32 x = sx + MOVE_OPS_LAVA[i][0];
		i32 y = sy + MOVE_OPS_LAVA[i][1];

		if (!is_inbounds(x, y))
		{
			continue;
		}

		if (i == 0 && !is_blocking(x, y))
		{
			p->next_pos = Vector2{ (f32)x, (f32)y };
			handle_particle_interaction(p, x, y);
			return false;
		}

		if (!is_blocking(x, y))
		{
			RAND_POOL_LAVA[push_ind][0] = x;
			RAND_POOL_LAVA[push_ind][1] = y;
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

	i32* rand_next_pos = RAND_POOL_LAVA[GetRandomValue(0, push_ind - 1)];
	p->next_pos = Vector2{ (f32)rand_next_pos[0], (f32)rand_next_pos[1] };
	handle_particle_interaction(p, rand_next_pos[0], rand_next_pos[1]);

	return false;
}

static bool is_blocking(i32 x, i32 y)
{
	i32 coord = y * MAX_WIDTH + x;
	return !(grid_arr[coord].type & LAVA_EXCLUSION_BITS);
}

static void handle_particle_interaction(Particle* p, i32 x, i32 y)
{
	i32 coord = y * MAX_WIDTH + x;
	f32 random_value = (f32)1 / (f32)GetRandomValue(0, 64);
	switch (grid_arr[coord].type)
	{
	case ACID:
		init_func[I_STONE](p);
		init_func[I_TOXIC_GAS](&grid_arr[coord]);
		break;
	case WATER:
		init_func[I_STONE](p);
		init_func[I_STEAM](&grid_arr[coord]);
		break;
	case AIR:
		break;
	case SAND:
		init_func[I_AIR](&grid_arr[coord]);
		if (random_value > 0.5)
		{
			init_func[I_STONE](p);
		}
		break;
	case WOOD:
		init_func[I_FIRE](&grid_arr[coord]);
		p->next_pos = p->pos;
		break;
	default:
		init_func[I_LAVA](&grid_arr[coord]);
		break;
	}
}
