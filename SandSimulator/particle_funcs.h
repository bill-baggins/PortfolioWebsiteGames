#ifndef PARTICLE_FUNCS_H
#define PARTICLE_FUNCS_H

#include "raylib.h"
#include "world.h"
#include "mytypes.h"
#include <array>

enum ParticleType
{
	AIR = 1 << 1,
	SAND = 1 << 2,
	WATER = 1 << 3,
	STONE = 1 << 4,
};

enum ParticleIndex
{
	I_AIR,
	I_SAND,
	I_WATER,
	I_STONE,
};

struct Particle
{
	Vector2 pos;
	Vector2 next_pos;
	Vector2 vel;
	Texture* texture;

	ParticleType type;
	ParticleIndex index;
	Color color;
	Color offset;
};

inline std::array<Particle, MAX_WIDTH* MAX_HEIGHT> grid_arr;

// Helper functions
bool is_inbounds_v(Vector2 pos);
bool is_inbounds(i32 x, i32 y);

void ParticleType_snprintf(char *buffer, usize size, char *fmt, ParticleIndex type);

void Particle_init(Particle* p, ParticleType type, ParticleIndex index, Color color, Vector2 pos, Texture* texture);
void Particle_free(Particle* p);

void init_air(Particle* p);
void update_draw_air(Particle* p, f32 dt);

void init_sand(Particle* p);
void update_draw_sand(Particle* p, f32 dt);

void init_water(Particle* p);
void update_draw_water(Particle* p, f32 dt);

void init_stone(Particle* p);
void update_draw_stone(Particle* p, f32 dt);

typedef void (*ParticleInitDelegate)(Particle*);
typedef void (*ParticleUpdateDrawDelegate)(Particle*, f32 dt);

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


#endif
