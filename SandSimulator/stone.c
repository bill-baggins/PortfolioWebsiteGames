#include "particle_funcs.h"

void init_stone(Particle* p, World* w)
{
	p->color = DARKGRAY;
	p->type = STONE;
}

void update_stone(Particle* p, World* w)
{
	
	p->next_pos = p->pos;
}

void draw_stone(Particle* p, World* w)
{
	Vector2 draw_pos = (Vector2){ p->pos.x * PIXEL_WIDTH, p->pos.y * PIXEL_HEIGHT };
	DrawTextureV(*p->texture, draw_pos, p->color);
}

void deinit_stone(Particle* p, World* w)
{

}
