#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include "mytypes.h"
#include "globals.h"
#include <cstdio>

struct World
{
	Texture sand_texture;
	Color background_color;
	usize particle_types;

	f32 blob_radius;
	enum ParticleIndex type_index;
	bool show_window;
};

void World_init(World* w);
void World_update_draw(World* w, f32 dt);
void World_deinit(World* w);

#endif
