#include "particle_funcs.h"


void init_air(Particle* p)
{
	p->type = AIR;
	p->index = I_AIR;
	p->color = Color{};
	p->health = 0;
	p->vel = Vector2{};
}

void update_air(Particle* p, f32 dt)
{

}

void draw_air(Particle* p, f32 dt)
{

}

