#include "particle_funcs.h"

void init_stone(Particle* p)
{
	p->type = STONE;
	p->index = I_STONE;
	p->color = STONE_COLOR;
	p->color = Color{
		p->color.r,
		(u8)(p->color.g + p->offset.g),
		(u8)(p->color.b + p->offset.b),
		255
	};
	p->health = 200;
	p->vel = Vector2{};
}

void update_stone(Particle* p, f32 dt)
{
	p->next_pos = p->pos;
}


void draw_stone(Particle* p, f32 dt)
{
	Vector2 draw_pos = Vector2{ p->pos.x * PIXEL_WIDTH, p->pos.y * PIXEL_HEIGHT };
	DrawTextureV(*p->texture, draw_pos, p->color);
}
