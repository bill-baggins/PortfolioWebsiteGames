#include "menu.h"
#include "mytypes.h"
#include "logging.h"
#include "misc_util.h"
#include "globals.h"

#include "selector.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "rlImGui.h"

#include "stb_ds.h"


// internally how the GUI will keep track of the saved rectangles
typedef struct TileGroup
{
	str_t name;
	Rectangle* rect_arr;
} TileGroup;

static void TileGroup_init(TileGroup* t, str_t name);
static void TileGroup_deinit(TileGroup* t);

// Private methods that operate on the Menu structure.
static void Menu_draw_layout(Menu* m);
static void Menu_add_selected_rectangle(Menu* m, Rectangle **rects);

// Miscellaenous helper methods.
static i32 Menu_add_entered_group(Menu* m, str_t err_buf);
static i32 callback_stub(ImGuiInputTextCallbackData* data);

void Menu_init(Menu* m, Selector* selector, Vector2 offset)
{
	m->gui_viewport = LoadRenderTexture(GUI_VIEWPORT_WIDTH, GUI_VIEWPORT_HEIGHT);
	m->gui_viewport_rect = (Rectangle){
		0, 0, (float)m->gui_viewport.texture.width, (float)-m->gui_viewport.texture.height
	};

	m->window_flags = 0;
	m->window_flags |= ImGuiWindowFlags_NoCollapse;

	m->offset = offset;

	m->groups_arr = NULL;
	m->show_group_name_window = false;
	memset(m->group_name_buffer, 0, MAX_GROUP_NAME_LENGTH);

	memset(m->err_buf, 0, MAX_ERROR_LENGTH);
	m->display_error = true;
	
	m->selector = selector;
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
	for (i32 i = 0; i < arrlen(m->groups_arr); i++)
	{
		TileGroup_deinit(&m->groups_arr[i]);
	}

	arrfree(m->groups_arr);
	UnloadRenderTexture(m->gui_viewport);
}

static void Menu_draw_layout(Menu* m)
{
	rlImGuiBegin();

	igSetNextWindowSize((ImVec2) { GUI_VIEWPORT_WIDTH, GUI_VIEWPORT_HEIGHT }, 0);
	igSetNextWindowPos((ImVec2) { 0, 0}, 0, (ImVec2) { 0, 0 });
	igBegin("Atlas Addresser", NULL, m->window_flags);
	{
		// Trying to set "NoMove" in the Menu_init() function shifts the window away from
		// <0, 0> and locks it. Thus, it needs to be done here.
		if ((m->window_flags & ImGuiWindowFlags_NoMove) == 0)
		{
			m->window_flags |= ImGuiWindowFlags_NoMove;
			m->window_flags |= ImGuiWindowFlags_NoResize;
		}

		if (igButton("Add Group", (ImVec2) { 80.f, 20.f }))
		{
			m->show_group_name_window = true;
		}

		if (m->show_group_name_window)
		{
			igSetNextWindowPos((ImVec2) { 50.f, 50.f }, ImGuiCond_FirstUseEver, (ImVec2) { 0 });
			igSetNextWindowSize((ImVec2) { 200.f, 80.f }, ImGuiCond_FirstUseEver);

			i32 group_win_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			igBegin("Group Name", &m->show_group_name_window, group_win_flags);
			{
				// Add this temporarily to prevent this current window from unfocusing.
				m->window_flags |= ImGuiWindowFlags_NoMouseInputs;

				i32 text_flags = 0;
				text_flags |= ImGuiInputTextFlags_EnterReturnsTrue;
				text_flags |= ImGuiInputTextFlags_CallbackCompletion;
				bool enter_key_pressed = igInputText("Enter Here", m->group_name_buffer, MAX_GROUP_NAME_LENGTH, text_flags, callback_stub, (void*)m);
				bool button_pressed = igButton("OK", (ImVec2) { 80.f, 20.f });
				if (enter_key_pressed || button_pressed)
				{
					i32 return_code = Menu_add_entered_group(m, m->err_buf);
					if (return_code == -1)
					{
						m->display_error = true;
					}
					else
					{
						m->display_error = false;
						memset(m->err_buf, 0, MAX_GROUP_NAME_LENGTH);
						m->show_group_name_window = false;
					}
				}

				if (m->display_error)
				{
					igText(m->err_buf);
				}
			}
			igEnd();
		}

		if (!m->show_group_name_window)
		{
			// Re-enable the mouse inputs when a name is chosen, or
			// when the group name window is closed.
			m->window_flags &= ~(ImGuiWindowFlags_NoMouseInputs);
			memset(m->err_buf, 0, MAX_ERROR_LENGTH);
			memset(m->group_name_buffer, 0, MAX_GROUP_NAME_LENGTH);
		}

		// Lists out the tile groups currently in use.
		for (i32 i = 0; i < arrlen(m->groups_arr); i++)
		{
			char rectangle_string[32] = { 0 };
			memset(rectangle_string, 0, 32);

			TileGroup tile_group = m->groups_arr[i];
			if (igTreeNode_Str(tile_group.name))
			{
				Rectangle* rects = tile_group.rect_arr;
				igText("Rectangles: %d", arrlen(rects));
				for (i32 i = 0; i < arrlen(rects); i++)
				{
					Rectangle_fmt(rectangle_string, rects[i]);
					igText(rectangle_string);
				}

				if (igButton("Add Selected", (ImVec2) { 100.f, 20.f }))
				{
					Menu_add_selected_rectangle(m, &m->groups_arr[i].rect_arr);
				}

				igSameLine(100.f, 50.f);
				if (igButton("Remove", (ImVec2) { 100.f, 20.f }))
				{
					TileGroup_deinit(&m->groups_arr[i]);
					arrdel(m->groups_arr, i);
					igTreePop();
					break;
				}

				igSeparator();
				igTreePop();
			}
		}
	}
	igEnd();


	rlImGuiEnd();
}

static i32 callback_stub(ImGuiInputTextCallbackData* data)
{
	// I don't do anything :p

	return 0;
}

static void Menu_add_selected_rectangle(Menu* m, Rectangle **rects)
{
	Selector* s = m->selector;
	if (!Rectangle_equals(s->selection, (Rectangle){ 0 }))
	{
		arrpush((*rects), s->selection);
	}
}

static i32 Menu_add_entered_group(Menu* m, str_t err_buf)
{
	usize new_name_len = strlen(m->group_name_buffer);

	if (new_name_len == 0)
	{
		snprintf(err_buf, MAX_ERROR_LENGTH, "Tile Group name is empty!");
		return -1;
	}
	
	// No spaces please
	for (i32 i = 0; i < new_name_len; i++)
	{
		str_t str = m->group_name_buffer;
		if (str[i] == ' ')
		{
			snprintf(err_buf, MAX_ERROR_LENGTH, "Name cannot contain spaces!");
			return -1;
		}
	}

	// Check for duplicates.
	for (i32 i = 0; i < arrlen(m->groups_arr); i++)
	{
		str_t current = m->groups_arr[i].name;
		usize current_name_len = strlen(current);
		if (current_name_len == new_name_len && strncmp(current, m->group_name_buffer, current_name_len) == 0)
		{
			snprintf(err_buf, MAX_ERROR_LENGTH, "A Tile Group with that name already exists.");
			return -1;
		}
	}

	TileGroup group = { 0 };
	TileGroup_init(&group, m->group_name_buffer);

	arrpush(m->groups_arr, group);

	memset(m->group_name_buffer, 0, MAX_GROUP_NAME_LENGTH);

	return 0;
}

static void TileGroup_init(TileGroup* t, str_t name)
{
	t->name = string_copy(name);
	t->rect_arr = NULL;
}

static void TileGroup_deinit(TileGroup* t)
{
	free(t->name);
	arrfree(t->rect_arr);
}
