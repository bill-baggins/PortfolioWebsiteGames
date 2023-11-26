#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "mytypes.h"
#include "globals.h"
#include <stdbool.h>

typedef struct Menu 
{
	RenderTexture gui_viewport; // needs unloading
	Rectangle gui_viewport_rect;

	i32 window_flags;

	Vector2 offset;
	bool show_group_name_window;

	char group_name_buffer[MAX_GROUP_NAME_LENGTH];
	
	char err_buf[MAX_ERROR_LENGTH];
	bool display_error;

	struct Selector* selector;
	struct TileGroup* groups_arr; // needs arrfree
} Menu;

void Menu_init(Menu* m, struct Selector *s, Vector2 offset);
void Menu_update(Menu* m, f32 dt);
void Menu_draw(Menu* m);
void Menu_deinit(Menu* m);

#endif

