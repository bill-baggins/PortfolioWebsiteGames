#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "raylib.h"

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "mytypes.h"

#define MAX_WIDTH 600
#define MAX_HEIGHT 360

typedef enum ParticleType
{
	AIR,
	SAND,
	WATER
} ParticleType;

// If I want a growable multi-dimensional array
// of particle IDs, consider adding a chunk struct that
// can then be dynamically added to the world grid as needed.
// For now, I'll just implement it as a stack-allocated 2d array.

typedef struct Particle
{
	ParticleType type;
} Particle;

typedef struct WorldGrid
{
	Particle* grid_arr[MAX_HEIGHT];
} WorldGrid;

void WorldGrid_init(WorldGrid* w);
void WorldGrid_deinit(WorldGrid* w);

typedef void (*ParticleUpdateDelegate)(Particle*, WorldGrid*);

void update_air(Particle* p, WorldGrid* w)
{
	puts("updating air");
}

void update_sand(Particle* p, WorldGrid* w)
{
	puts("updating sand");
}

void update_water(Particle* p, WorldGrid* w)
{
	puts("updating water");
}

static ParticleUpdateDelegate funcs[] = {
	update_air,
	update_sand,
	update_water
};


int main(int argc, char* argv[]) 
{
	srand(time(NULL));

	WorldGrid world = { 0 };
	WorldGrid_init(&world);

	world.grid_arr[12][2].type = SAND;

	for (i32 i = 0; i < MAX_HEIGHT; i++)
	{
		for (i32 j = 0; j < MAX_WIDTH; j++)
		{
			Particle* arr = world.grid_arr[i];
			if (arr == NULL) continue;
			Particle particle = arr[j];

			if (j == 10) break;

			funcs[particle.type](&particle, &world);
		}
	}

	WorldGrid_deinit(&world);

	return 0;
}

void WorldGrid_init(WorldGrid* w)
{
	printf("Size of world struct on stack: %llu\n", sizeof(*w));
	memset(w->grid_arr, 0, sizeof(Particle*) * MAX_HEIGHT);
	for (i32 i = 0; i < MAX_HEIGHT; i++)
	{
		arrsetcap(w->grid_arr[i], MAX_WIDTH);
		
		Particle* arr = w->grid_arr[i];
		for (i32 j = 0; j < MAX_WIDTH; j++)
		{
			arr[j] = (Particle){ (ParticleType) rand() % 3 };
		}
	}
}

void WorldGrid_deinit(WorldGrid* w)
{
	for (i32 i = 0; i < MAX_HEIGHT; i++)
	{
		arrfree(w->grid_arr[i]);
	}
}