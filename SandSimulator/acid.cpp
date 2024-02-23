#include "particle_funcs.h"
#include "world.h"
#include "globals.h"
#include "raymath.h"

#include <cstdio>

static void calculate_next_move(Particle* p);
static bool is_blocking(i32 x, i32 y);

static i32 MOVE_OPS_ACID[][2] = {
	{ 0, 1},
	{ -1, 1 },
	{ 1, 1},
	{ -1, 0 },
	{ 1, 0 },
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
	p->vel = Vector2{};
}

void update_acid(Particle* p, f32 dt)
{
	p->vel.y += std::min(GRAVITY * dt, TERM_VEL);
	calculate_next_move(p);
}

void draw_acid(Particle* p, f32 dt)
{
	Vector2 draw_pos = Vector2{ p->pos.x * PIXEL_WIDTH, p->pos.y * PIXEL_HEIGHT };
	DrawTextureV(*p->texture, draw_pos, p->color);
}

static void calculate_next_move(Particle* p)
{
	i32 sx = p->pos.x;
	i32 sy = p->pos.y;

	f32 random_value = (f32)1 / (f32)GetRandomValue(0, 64);

	p->next_pos = p->pos;

	for (i32 i = 0; i < 5; i++)
	{
		bool reached_end_of_loop = false;
		i32 y_limit = ceilf(p->vel.y) + 1;
		for (i32 j = 0; j < y_limit; j++)
		{
			i32 dx = sx + MOVE_OPS_ACID[i][0] * (j * 1.2f);
			i32 dy = sy + MOVE_OPS_ACID[i][1] * j;
			if (j + 1 == y_limit)
			{
				reached_end_of_loop = true;
			}

			if (is_inbounds(dx, dy) && !is_blocking(dx, dy))
			{
				i32 coord = dy * MAX_WIDTH + dx;
				p->next_pos = Vector2{ (f32)dx, (f32)dy };
				if (grid_arr[coord].type != AIR)
				{
					init_func[I_TOXIC_GAS](&grid_arr[coord]);
					if (random_value > 0.0675f)
					{
						init_func[I_AIR](p);
					}
				}
			}

		}

		if (reached_end_of_loop && !Vector2Equals(p->next_pos, p->pos))
		{
			return;
		}
	}

	if (Vector2Equals(p->next_pos, p->pos))
	{
		p->vel.y /= 1.025f;
	}
}

static bool is_blocking(i32 x, i32 y)
{
	i32 coord = y * MAX_WIDTH + x;
	return !(grid_arr[coord].type & EXCLUSION_BITS_ACID);
}