#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include "mytypes.h"
#include "globals.h"

typedef struct World
{
	Color background_color;
	Texture sand_texture;
	enum ParticleType current_type;
	f32 blob_radius;

	usize particle_types;

	struct Particle** grid_arr[MAX_HEIGHT];
} World;

void World_init(World* w);
void World_update(World* w, f32 dt);
void World_draw(World* w);
void World_deinit(World* w);

#endif
