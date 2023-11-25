#include "menu.h"
#include "mytypes.h"
#include "logging.h"
#include "misc_util.h"
#include "globals.h"
#include "selector.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "raylib.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "rlImGui.h"

#include "stb_ds.h"

// internally how the GUI will keep track of the saved
// rectangles.
typedef struct TileGroup
{
	str_t group_name;
	Rectangle* tile_rect_arr;
} TileGroup;

static void TileGroup_init(TileGroup* t, str_t group_name);
static void TileGroup_free(TileGroup* t);
// end TileGroup functions.


// Private methods that operate on the Menu structure.
static void Menu_draw_layout(Menu* menu);

void Menu_init(Menu* m, Selector *selector, Vector2 offset)
{
	m->gui_viewport = LoadRenderTexture(GUI_VIEWPORT_WIDTH, GUI_VIEWPORT_HEIGHT);
	m->gui_viewport_rect = (Rectangle){
		0, 0, (float)m->gui_viewport.texture.width, (float)-m->gui_viewport.texture.height
	};

	m->window_flags |= ImGuiWindowFlags_None;
	m->offset = offset;
	m->selector = selector;

	m->tile_group_arr = NULL;
}

void Menu_update(Menu* m, f32 dt)
{

}

void Menu_draw(Menu* m)
{
	BeginTextureMode(m->gui_viewport);
	ClearBackground(GRAY);
	
	Menu_draw_layout(m);

	EndTextureMode(); // End GUI viewport

	DrawTextureRec(m->gui_viewport.texture, m->gui_viewport_rect, (Vector2) { 0 }, WHITE);
}

void Menu_deinit(Menu* m)
{
	UnloadRenderTexture(m->gui_viewport);
}

static void Menu_draw_layout(Menu* menu)
{
	rlImGuiBegin();
	
	ImGuiIO* io = igGetIO();
	// igBegin("Atlas Creator", NULL, 0);
	// igShowDemoWindow(NULL);

	igSetNextWindowSize((ImVec2) { GUI_VIEWPORT_WIDTH-200, GUI_VIEWPORT_HEIGHT-400 }, ImGuiCond_FirstUseEver);
	igBegin("Atlas Addresser", NULL, ImGuiWindowFlags_None);
	if (igButton("Add Group", (ImVec2) { 80.f, 20.f }))
	{
		
	}
	igEnd();


	rlImGuiEnd();
}

static void TileGroup_init(TileGroup* t, str_t group_name)
{
	t->group_name = string_copy(group_name);
	t->tile_rect_arr = NULL;
}

static void TileGroup_free(TileGroup* t)
{
	free(t->group_name);
	arrfree(t->tile_rect_arr);
}
