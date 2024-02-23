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
	i32 health;
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
void init_sand(Particle* p);
void init_water(Particle* p);
void init_stone(Particle* p);
void init_acid(Particle* p);
void init_toxic_gas(Particle* p);
void init_steam(Particle* p);
void init_lava(Particle* p);
void init_fire(Particle* p);
void init_smoke(Particle* p);
void init_wood(Particle* p);

void update_air(Particle* p, f32 dt);
void update_sand(Particle* p, f32 dt);
void update_water(Particle* p, f32 dt);
void update_stone(Particle* p, f32 dt);
void update_acid(Particle* p, f32 dt);
void update_toxic_gas(Particle* p, f32 dt);
void update_steam(Particle* p, f32 dt);
void update_lava(Particle* p, f32 dt);
void update_fire(Particle* p, f32 dt);
void update_smoke(Particle* p, f32 dt);
void update_wood(Particle* p, f32 dt);

void draw_air(Particle* p, f32 dt);
void draw_sand(Particle* p, f32 dt);
void draw_water(Particle* p, f32 dt);
void draw_stone(Particle* p, f32 dt);
void draw_acid(Particle* p, f32 dt);
void draw_toxic_gas(Particle* p, f32 dt);
void draw_steam(Particle* p, f32 dt);
void draw_lava(Particle* p, f32 dt);
void draw_fire(Particle* p, f32 dt);
void draw_smoke(Particle* p, f32 dt);
void draw_wood(Particle* p, f32 dt);

typedef void (*ParticleInitDelegate)(Particle*);
typedef void (*ParticleUpdateDelegate)(Particle*, f32 dt);

typedef void (*ParticleDrawDelegate)(Particle*, f32 dt);

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

static ParticleUpdateDelegate update_func[] = {
	update_air,
	update_sand,
	update_water,
	update_stone,
	update_acid,
	update_toxic_gas,
	update_steam,
	update_lava,
	update_fire,
	update_smoke,
	update_wood,
};

static ParticleDrawDelegate draw_func[] = {
	draw_air,
	draw_sand,
	draw_water,
	draw_stone,
	draw_acid,
	draw_toxic_gas,
	draw_steam,
	draw_lava,
	draw_fire,
	draw_smoke,
	draw_wood,
};


#endif
