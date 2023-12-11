#ifndef PARTICLE_FUNCS_H
#define PARTICLE_FUNCS_H

#include "raylib.h"
#include "world.h"
#include "mytypes.h"

enum ParticleType
{
	AIR,
	SAND,
	WATER,
	STONE
};

struct Particle
{
	ParticleType type;
	Color color;
	Color offset;
	Vector2 pos;
	Vector2 next_pos;

	Texture* texture;
	void* special;
};

// Helper functions
bool is_inbounds_v(Vector2 pos);
bool is_inbounds(i32 x, i32 y);

void ParticleType_snprintf(char *buffer, usize size, char *fmt, ParticleType type);

Particle* Particle_new(ParticleType type, Color color, Vector2 pos, Texture *texture);
void Particle_free(Particle* p);

void init_air(Particle* p, World* w);
void update_draw_air(Particle* p, World* w, f32 dt);
void deinit_air(Particle* p, World* w);

void init_sand(Particle* p, World* w);
void update_draw_sand(Particle* p, World* w, f32 dt);
void deinit_sand(Particle* p, World* w);

void init_water(Particle* p, World* w);
void update_draw_water(Particle* p, World* w, f32 dt);
void deinit_water(Particle* p, World* w);

void init_stone(Particle* p, World* w);
void update_draw_stone(Particle* p, World* w, f32 dt);
void deinit_stone(Particle* p, World* w);

typedef void (*ParticleInitDelegate)(struct Particle*, struct World*);
typedef void (*ParticleUpdateDrawDelegate)(struct Particle*, struct World*, f32 dt);

static ParticleInitDelegate init_func[] = {
	init_air,
	init_sand,
	init_water,
	init_stone
};

static ParticleUpdateDrawDelegate update_draw_func[] = {
	update_draw_air,
	update_draw_sand,
	update_draw_water,
	update_draw_stone
};

static ParticleInitDelegate deinit_func[] = {
	deinit_air,
	deinit_sand,
	deinit_water,
	deinit_stone
};


#endif
