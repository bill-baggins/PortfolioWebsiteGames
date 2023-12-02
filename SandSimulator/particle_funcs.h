#ifndef PARTICLE_FUNCS_H
#define PARTICLE_FUNCS_H

#include "raylib.h"
#include "world.h"
#include "mytypes.h"

typedef enum ParticleType
{
	AIR,
	SAND,
	WATER,
	STONE
} ParticleType;

typedef struct Particle
{
	ParticleType type;
	Color color;
	Vector2 pos;
	Vector2 next_pos;

	Texture* texture;
	void* special;
} Particle;

// Helper functions
bool is_inbounds_v(Vector2 pos);
bool is_inbounds(i32 x, i32 y);

void ParticleType_snprintf(char *buffer, usize size, char *fmt, ParticleType type);

Particle* Particle_new(ParticleType type, Color color, Vector2 pos, Texture *texture);
void Particle_free(Particle* p);

void init_air(Particle* p, World* w);
void update_air(Particle* p, World* w);
void draw_air(Particle* p, World* w);
void deinit_air(Particle* p, World* w);

void init_sand(Particle* p, World* w);
void update_sand(Particle* p, World* w);
void draw_sand(Particle* p, World* w);
void deinit_sand(Particle* p, World* w);

void init_water(Particle* p, World* w);
void update_water(Particle* p, World* w);
void draw_water(Particle* p, World* w);
void deinit_water(Particle* p, World* w);

void init_stone(Particle* p, World* w);
void update_stone(Particle* p, World* w);
void draw_stone(Particle* p, World* w);
void deinit_stone(Particle* p, World* w);

typedef void (*ParticleDelegate)(struct Particle*, struct World*);

static ParticleDelegate init_func[] = {
	init_air,
	init_sand,
	init_water,
	init_stone
};

static ParticleDelegate update_func[] = {
	update_air,
	update_sand,
	update_water,
	update_stone
};

static ParticleDelegate draw_func[] = {
	draw_air,
	draw_sand,
	draw_water,
	draw_stone
};

static ParticleDelegate deinit_func[] = {
	deinit_air,
	deinit_sand,
	deinit_water,
	deinit_stone
};


#endif
