#include "particle_funcs.h"

void init_air(Particle* p)
{
	p->color = Color{};
	p->type = AIR;
	p->index = I_AIR;

}

void update_draw_air(Particle* p, f32 dt) 
{
}
