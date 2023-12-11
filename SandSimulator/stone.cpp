#include "particle_funcs.h"

void init_stone(Particle* p, World* w)
{
	p->type = STONE;
	p->color = DARKGRAY;
	p->color = Color{
		p->color.r,
		(u8)(p->color.g + p->offset.g),
		(u8)(p->color.b + p->offset.b),
		255
	};
}

void update_draw_stone(Particle* p, World* w, f32 dt)
{
	p->next_pos = p->pos;
	Vector2 draw_pos = Vector2{ p->pos.x * PIXEL_WIDTH, p->pos.y * PIXEL_HEIGHT };
	DrawTextureV(*p->texture, draw_pos, p->color);
}

void deinit_stone(Particle* p, World* w)
{

}
