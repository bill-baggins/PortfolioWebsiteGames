#include "particle_funcs.h"

void init_wood(Particle* p)
{
	p->type = WOOD;
	p->index = I_WOOD;
	p->color = WOOD_COLOR;
	p->color = Color{
		p->color.r,
		(u8)(p->color.g + p->offset.g),
		(u8)(p->color.b + p->offset.b),
		255
	};
	p->health = 50;
}

void update_draw_wood(Particle* p, f32 dt)
{
	p->next_pos = p->pos;
	Vector2 draw_pos = Vector2{ p->pos.x * PIXEL_WIDTH, p->pos.y * PIXEL_HEIGHT };
	DrawTextureV(*p->texture, draw_pos, p->color);
}
