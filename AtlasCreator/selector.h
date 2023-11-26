#ifndef SELECTOR_H
#define SELECTOR_H

#include "mytypes.h"
#include "raylib.h"

typedef struct Selector 
{
	Rectangle selection;
	Color selection_color;
	Color selection_outline_color;

	Vector2 current;
	Vector2 next;

	Camera2D camera;
	f32 scroll_factor;

	Texture atlas; // needs to be unloaded.

	RenderTexture image_viewport; // needs to be unloaded
	Rectangle image_viewport_rect;
	
	// For adjusting the mouse position based on
	// where the RenderTexture viewport is placed.
	Vector2 offset;

	struct Menu* menu;
} Selector;

// Selector* Selector_new(Selector* s);

// For initializing the contents of a value type.
void Selector_init(Selector* s, struct Menu* menu, Vector2 offset);
void Selector_update(Selector* s, f32 dt);
void Selector_draw(Selector* s);
void Selector_deinit(Selector* s);


#endif