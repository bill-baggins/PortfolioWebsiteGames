#include "particle_funcs.h"
#include "world.h"
#include "globals.h"

#include <stdio.h>

static void calculate_next_move(Particle* p, World* w);
static bool is_blocking(i32 x, i32 y, World* w);

#define EXCLUSIONLEN (sizeof(exclusion_list)/sizeof(exclusion_list[0]))

static ParticleType exclusion_list[] = {
	AIR,
};

void init_water(Particle* p, World* w)
{
	p->color = BLUE;
	p->type = WATER;
}

void update_water(Particle* p, World* w)
{
	calculate_next_move(p, w);
}

void draw_water(Particle *p, World *w)
{
	Vector2 draw_pos = (Vector2){ p->pos.x * PIXEL_WIDTH, p->pos.y * PIXEL_HEIGHT };
	DrawTextureV(*p->texture, draw_pos, p->color);
}

void deinit_water(Particle* p, World* w)
{

}

static void calculate_next_move(Particle* p, World* w)
{
	i32 sx = p->pos.x;
	i32 sy = p->pos.y;

	// Vertically out of bounds, do not consider.
	if (!is_inbounds(0, sy))
	{
		return;
	}

	i32 move_ops[][2] = {
		{ sx, sy + 1},
		{ sx - 1, sy + 1 },
		{ sx + 1, sy + 1},
		{ sx + 1, sy },
		{ sx - 1, sy },
	};

	i32 rand_pool[5][2] = { 0 };
	i32 push_ind = 0;

	for (i32 i = 0; i < 5; i++)
	{
		i32 x = move_ops[i][0];
		i32 y = move_ops[i][1];
		if (!is_inbounds(x, y))
		{
			continue;
		}

		if (i == 0 && !is_blocking(x, y, w))
		{
			p->next_pos = (Vector2){ x, y };
			return;
		}

		if (!is_blocking(x, y, w))
		{
			rand_pool[push_ind][0] = x;
			rand_pool[push_ind][1] = y;
			push_ind++;
		}
	}

	// means that no spot was found, just set the next pos to
	// what it is.
	if (push_ind == 0)
	{
		p->next_pos = p->pos;
		return;
	}

	i32* rand_next_pos = rand_pool[GetRandomValue(0, push_ind - 1)];
	p->next_pos = (Vector2){ rand_next_pos[0], rand_next_pos[1] };
}

static bool is_blocking(i32 x, i32 y, World* w)
{
	if (!is_inbounds(x, y)) return true;

	Particle* p = w->grid_arr[y][x];
	for (i32 i = 0; i < EXCLUSIONLEN; i++)
	{
		if (p->type == exclusion_list[i])
		{
			return false;
		}
	}

	return true;
}
