#include "particle_funcs.h"
#include "raylib.h"
#include <stdlib.h>
#include "logging.h"


Particle* Particle_new(ParticleType type, Color color, Vector2 pos, Texture* texture)
{
	Particle* p = malloc(sizeof(Particle));
	if (p == NULL)
	{
		LOG_FATAL("oops, ran out of memory allocating a particle. get more ram lol");
	}

	p->type = type;
	p->color = color;
	p->pos = pos;
	p->next_pos = pos;
	p->texture = texture;
	p->special = NULL;

	return p;
}

void Particle_free(Particle* p)
{
	free(p->special);
	free(p);
}
