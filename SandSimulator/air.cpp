#include "particle_funcs.h"

void init_air(Particle* p)
{
	p->color = Color{};
	p->type = AIR;
	p->index = I_AIR;
	p->health = 0;
}

void update_draw_air(Particle* p, f32 dt) 
{
}
