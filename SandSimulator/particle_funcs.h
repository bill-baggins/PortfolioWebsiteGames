#ifndef PARTICLE_FUNCS_H
#define PARTICLE_FUNCS_H

#include "raylib.h"
#include "world.h"
#include "mytypes.h"
#include <array>

#define TAN (CLITERAL(Color){ 240, 220, 180, 255 })
#define STONE_COLOR (DARKGRAY)
#define WATER_COLOR (BLUE)
#define SAND_COLOR (TAN)
#define ACID_COLOR (LIME)
#define TOXIC_GAS_COLOR ((CLITERAL(Color){LIME.r, LIME.g, LIME.b, 100}))
#define STEAM_COLOR (DARKGRAY)
#define FIRE_COLOR (CLITERAL(Color){ 200, 121, 0, 255 })
#define LAVA_COLOR (ORANGE)
#define WOOD_COLOR (BROWN)
#define SMOKE_COLOR ((CLITERAL(Color){20, 20, 20, 100}))

// General TODO:
// Consider using an algorithm where dynamic particles never displace one another or interact with each other
// initially when their next position is being determined. Use a seperate loop that makes more advanced decisions about
// where to move the particle to or for what particle should displace what.
// In essence, instead of having each particle struct decide for themselves whether they should display another particle,
// have one for loop that decides it for all particles at once.

enum ParticleType
{
	AIR = 1 << 1,
	SAND = 1 << 2,
	WATER = 1 << 3,
	STONE = 1 << 4,
	ACID = 1 << 5,
	TOXIC_GAS = 1 << 6,
	STEAM = 1 << 7,
	LAVA = 1 << 8,
	FIRE = 1 << 9,
	SMOKE = 1 << 10,
	WOOD = 1 << 11,
};

enum ParticleIndex
{
	I_AIR,
	I_SAND,
	I_WATER,
	I_STONE,
	I_ACID,
	I_TOXIC_GAS,
	I_STEAM,
	I_LAVA,
	I_FIRE,
	I_SMOKE,
	I_WOOD,
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

inline Particle grid_arr[MAX_WIDTH * MAX_HEIGHT];

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

void init_acid(Particle* p);
void update_draw_acid(Particle* p, f32 dt);

void init_toxic_gas(Particle* p);
void update_draw_toxic_gas(Particle* p, f32 dt);

void init_steam(Particle* p);
void update_draw_steam(Particle* p, f32 dt);

void init_lava(Particle* p);
void update_draw_lava(Particle* p, f32 dt);

void init_fire(Particle* p);
void update_draw_fire(Particle* p, f32 dt);

void init_smoke(Particle* p);
void update_draw_smoke(Particle* p, f32 dt);

void init_wood(Particle* p);
void update_draw_wood(Particle* p, f32 dt);

typedef void (*ParticleInitDelegate)(Particle*);
typedef void (*ParticleUpdateDrawDelegate)(Particle*, f32 dt);

static Color PARTICLE_COLORS[] = {
	Color{},
	SAND_COLOR,
	WATER_COLOR,
	STONE_COLOR,
	ACID_COLOR,
	TOXIC_GAS_COLOR,
	STEAM_COLOR,
	LAVA_COLOR,
	FIRE_COLOR,
	SMOKE_COLOR,
	WOOD_COLOR
};

static usize PARTICLE_COLORS_LENGTH = sizeof(PARTICLE_COLORS) / sizeof(Color);

static ParticleInitDelegate init_func[] = {
	init_air,
	init_sand,
	init_water,
	init_stone,
	init_acid,
	init_toxic_gas,
	init_steam,
	init_lava,
	init_fire,
	init_smoke,
	init_wood,
};

static ParticleUpdateDrawDelegate update_draw_func[] = {
	update_draw_air,
	update_draw_sand,
	update_draw_water,
	update_draw_stone,
	update_draw_acid,
	update_draw_toxic_gas,
	update_draw_steam,
	update_draw_lava,
	update_draw_fire,
	update_draw_smoke,
	update_draw_wood,
};


#endif
