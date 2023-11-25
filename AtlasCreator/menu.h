#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "mytypes.h"

typedef struct Menu 
{
	RenderTexture gui_viewport;
	Rectangle gui_viewport_rect;

	i32 window_flags;

	Vector2 offset;

	struct Selector* selector;
	struct TileGroup* tile_group_arr;
} Menu;

void Menu_init(Menu* m, struct Selector *selector, Vector2 offset);
void Menu_update(Menu* m, f32 dt);
void Menu_draw(Menu* m);
void Menu_deinit(Menu* m);

#endif