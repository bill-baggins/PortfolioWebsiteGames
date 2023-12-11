#include "particle_funcs.h"

void init_air(Particle* p, World* w)
{
	p->color = Color{};
	p->type = AIR;
	p->texture = &w->sand_texture;
}

void update_draw_air(Particle* p, World* w, f32 dt) 
{
}

void deinit_air(Particle* p, World* w) {}
