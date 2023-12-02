#include "particle_funcs.h"

void init_air(Particle* p, World* w)
{
	p->color = (Color){ 0 };
	p->type = AIR;
	p->texture = &w->sand_texture;
}

void update_air(Particle* p, World* w) 
{
}

void draw_air(Particle* p, World* w) {}

void deinit_air(Particle* p, World* w) {}
