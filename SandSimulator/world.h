#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include "mytypes.h"
#include "globals.h"
#include <cstdio>

struct World
{
	Color background_color;
	Texture sand_texture;
	enum ParticleType current_type;
	f32 blob_radius;

	usize particle_types;

	struct Particle** grid_arr[MAX_HEIGHT];
};

void World_init(World* w);
void World_update_draw(World* w, f32 dt);
void World_deinit(World* w);

#endif
