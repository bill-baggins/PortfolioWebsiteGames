#include "particle_funcs.h"
#include "raylib.h"
#include <cstdlib>
#include <cstdio>
#include "logging.h"


void Particle_init(Particle *p, ParticleType type, ParticleIndex index, Color color, Vector2 pos, Texture* texture)
{
	i32 offset = 10;

	p->type = type;
	p->index = index;
	p->color = color;
	p->offset = Color{ 
		(u8)GetRandomValue(-offset, offset), 
		(u8)GetRandomValue(-offset, offset), 
		(u8)GetRandomValue(-offset, offset),
		255 
	};
	p->pos = pos;
	p->next_pos = pos;
	p->health = 0;
	p->vel = Vector2{};
	p->texture = texture;
}

void Particle_free(Particle* p)
{
	
}
