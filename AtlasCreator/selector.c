#include "selector.h"
#include "menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"

#include "logging.h"
#include "globals.h"

#define TRANSPARENT_SKY_BLUE (CLITERAL(Color){SKYBLUE.r, SKYBLUE.g, SKYBLUE.b, 100})
#define TRANSPARENT_DARK_BLUE (CLITERAL(Color){DARKBLUE.r, DARKBLUE.g, DARKBLUE.b, 100})
#define TRANSPARENT_VIOLET (CLITERAL(Color){VIOLET.r, VIOLET.g, VIOLET.b, 100})

void Selector_init(Selector* s, Menu *menu, Vector2 offset)
{
	if (NULL == s) 
	{
		LOG_WARN("Selector struct that got passed in is null");
		return;
	}

	Image cursor_im = GenImageColor(1, 1, TRANSPARENT_SKY_BLUE);
	s->cursor = LoadTextureFromImage(cursor_im);
	UnloadImage(cursor_im);
	s->cursor_position = (Vector2){ 0 };
	
	
	s->selection = (Rectangle){ 0 };
	s->selection_color = TRANSPARENT_SKY_BLUE;
	s->selection_outline_color = TRANSPARENT_DARK_BLUE;
	s->current = (Vector2){ 0 };
	s->next = (Vector2){ 0 };

	s->scroll_factor = 1.10f;

	s->camera = (Camera2D){ 0 };
	s->camera.zoom = 1.0;

	// #93bbec = background color of the atlas image.
	Image atlas = LoadImage("assets/minesweeper.png");
	ImageColorReplace(&atlas, (Color) { 0x93, 0xbb, 0xec, 255 }, (Color) { 0 });
	s->atlas = LoadTextureFromImage(atlas);
	UnloadImage(atlas);

	s->image_viewport = LoadRenderTexture(IMAGE_VIEWPORT_WIDTH, IMAGE_VIEWPORT_HEIGHT);

	s->image_viewport_rect = (Rectangle){
		0, 0, (f32)s->image_viewport.texture.width, (f32)-s->image_viewport.texture.height
	};

	s->offset = offset;
	s->menu = menu;
}

void Selector_update(Selector* s, f32 dt)
{
	Vector2 mouse_pos = GetMousePosition();
	mouse_pos.x -= s->offset.x;
	mouse_pos.y -= s->offset.y;

	// Bounds checking on the render texture so that the mouse
	// position isn't out of bounds.
	if ((mouse_pos.x < 0 || mouse_pos.x > s->image_viewport.texture.width) ||
		(mouse_pos.y < 0 || mouse_pos.y > s->image_viewport.texture.height))
	{
		return;
	}

	s->cursor_position = GetScreenToWorld2D(mouse_pos, s->camera);
	s->cursor_position.x = roundf(s->cursor_position.x);
	s->cursor_position.y = roundf(s->cursor_position.y);
	

	if (IsKeyPressed(KEY_Z))
	{
		LOG_DEBUG("Camera information below:");
		LOG_DEBUG("Camera Offset: <%.2f, %.2f>", s->camera.offset.x, s->camera.offset.y);
		LOG_DEBUG("Camera Target: <%.2f, %.2f>", s->camera.target.x, s->camera.target.y);
		LOG_DEBUG("Camera Zoom: %.5f", s->camera.zoom);
		puts("");
		LOG_DEBUG("s->current position: <%d, %d>", (i32)roundf(s->current.x), (i32)roundf(s->current.y));
		LOG_DEBUG("s->next position: <%d, %d>", (i32)roundf(s->next.x), (i32)roundf(s->next.y));
		puts("");
	}

	// for dragging the camera
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
	{
		Vector2 mouse_delta = GetMouseDelta();
		s->camera.target.x += -mouse_delta.x * (1 / s->camera.zoom);
		s->camera.target.y += -mouse_delta.y * (1 / s->camera.zoom);
	}

	// Grabs the first point if next is zero, otherwise grabs the new next position.
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		if (!Vector2Equals(s->next, (Vector2) { 0 }) && Vector2Equals(s->current, (Vector2) { 0 }))
		{
			s->current = GetScreenToWorld2D(mouse_pos, s->camera);
			s->current.x = roundf(s->current.x);
			s->current.y = roundf(s->current.y);

			s->selection = (Rectangle){ s->current.x, s->current.y, 1, 1 };
		}
		else
		{
			s->next = GetScreenToWorld2D(mouse_pos, s->camera);
			s->next.x = roundf(s->next.x);
			s->next.y = roundf(s->next.y);
		}
	}

	// Next point and current point is used to construct a rectangle.
	if (!Vector2Equals(s->current, (Vector2) { 0 }) && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		s->next = GetScreenToWorld2D(mouse_pos, s->camera);
		s->selection_color = TRANSPARENT_VIOLET;

		f32 width = roundf(s->next.x - s->current.x);
		f32 height = roundf(s->next.y - s->current.y);

		s->current.x = roundf(s->current.x);
		s->current.y = roundf(s->current.y);
		s->next.x = roundf(s->next.x);
		s->next.y = roundf(s->next.y);

		if (width < 0 && height < 0)
		{
			s->selection = (Rectangle){ s->next.x, s->next.y, -width, -height };
		}
		else if (height < 0)
		{
			s->selection = (Rectangle){ s->current.x, s->next.y, width, -height };
		}
		else if (width < 0)
		{
			s->selection = (Rectangle){ s->next.x, s->current.y, -width, height };
		}
		else
		{
			s->selection = (Rectangle){ s->current.x, s->current.y, width, height };
		}
	}

	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
	{
		s->next = (Vector2){ 0 };
		s->selection_color = TRANSPARENT_SKY_BLUE;
	}

	// For zooming in and out.
	if (IsKeyDown(KEY_LEFT_CONTROL))
	{
		f32 scroll = GetMouseWheelMove();
		if (scroll != 0)
		{
			scroll *= s->scroll_factor;
			if (scroll < 0)
			{
				s->camera.zoom = max(s->camera.zoom / -scroll, MIN_ZOOM);
			}
			else if (scroll > 0)
			{
				s->camera.zoom = min(s->camera.zoom * scroll, MAX_ZOOM);
			}
			
			Vector2 pos = GetScreenToWorld2D(mouse_pos, s->camera);
			s->camera.offset = mouse_pos;
			s->camera.target = pos;
		}
	}

	if (IsKeyPressed(KEY_ESCAPE))
	{
		s->next = (Vector2){ 0 };
		s->current = (Vector2){ 0 };
		s->selection = (Rectangle){ 0 };
	}

}

void Selector_draw(Selector* s)
{
	BeginTextureMode(s->image_viewport);
	ClearBackground(DARKGRAY);
	BeginMode2D(s->camera);
	{
		DrawRectangleLines(0, 0, s->atlas.width, s->atlas.height, BLACK);
		DrawTexture(s->atlas, 0, 0, RAYWHITE);
		DrawRectangleLinesEx(s->selection, 1, s->selection_outline_color);
		DrawRectangleRec(s->selection, s->selection_color);
		DrawTextureV(s->cursor, s->cursor_position, WHITE);
	}
	EndMode2D();
	EndTextureMode(); // End image_viewport

	DrawTextureRec(s->image_viewport.texture, s->image_viewport_rect, (Vector2) { (f32)s->offset.x, 0 }, WHITE);
}

void Selector_deinit(Selector* s)
{
	UnloadTexture(s->atlas);
	UnloadRenderTexture(s->image_viewport);
}
