#include "particle_funcs.h"
#include "world.h"
#include "globals.h"

#include <cstdio>

static bool calculate_next_move(Particle* p);
static bool is_blocking(i32 x, i32 y);

static i32 MOVE_OPS_ACID[][2] = {
	{ 0, 1},
	{ -1, 1 },
	{ 1, 1},
	{ 1, 0 },
	{ -1, 0 },
};
static i32 RAND_POOL_ACID[5][2] = { 0 };

static i32 EXCLUSION_BITS_ACID = SAND | WATER | WOOD | AIR | STONE | STEAM | SMOKE | TOXIC_GAS;

void init_acid(Particle* p)
{
	p->type = ACID;
	p->index = I_ACID;
	p->color = ACID_COLOR;
	p->color.a = 200;
	p->color = Color{
		p->color.r,
		(u8)(p->color.g + p->offset.g),
		(u8)(p->color.b + p->offset.b),
		200
	};
	p->health = 0;
}

void update_draw_acid(Particle* p, f32 dt)
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
	f32 random_value = (f32)1 / (f32)GetRandomValue(0, 64);

	for (i32 i = 0; i < 5; i++)
	{
		i32 x = sx + MOVE_OPS_ACID[i][0];
		i32 y = sy + MOVE_OPS_ACID[i][1];

		if (!is_inbounds(x, y))
		{
			continue;
		}

		if (i == 0 && !is_blocking(x, y))
		{
			i32 coord = y * MAX_WIDTH + x;
			p->next_pos = Vector2{ (f32)x, (f32)y };
			if (grid_arr[coord].type != AIR)
			{
				init_func[I_TOXIC_GAS](&grid_arr[coord]);
				if (random_value > 0.0675f)
				{
					init_func[I_AIR](p);
				}
			}
			return false;
		}

		if (!is_blocking(x, y))
		{
			RAND_POOL_ACID[push_ind][0] = x;
			RAND_POOL_ACID[push_ind][1] = y;
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

	i32* rand_next_pos = RAND_POOL_ACID[GetRandomValue(0, push_ind - 1)];
	i32 coord = rand_next_pos[1] * MAX_WIDTH + rand_next_pos[0];
	p->next_pos = Vector2{ (f32)rand_next_pos[0], (f32)rand_next_pos[1] };
	if (grid_arr[coord].type != AIR)
	{
		init_func[I_TOXIC_GAS](&grid_arr[coord]);
		if (random_value > 0.0675f)
		{
			init_func[I_AIR](p);
		}
		
	}

	return false;
}

static bool is_blocking(i32 x, i32 y)
{
	i32 coord = y * MAX_WIDTH + x;
	return !(grid_arr[coord].type & EXCLUSION_BITS_ACID);
}