#include "texture_handler.h"

TextureHandler::TextureHandler(float scale)
{
	using enum TextureType;

	width = 30 * scale;
	height = 30 * scale;

	float w = width;
	float h = height;
	float gap = 3 * scale;
	
	Image im_atlas = LoadImage("resources/minesweeper.png");
	ImageResize(&im_atlas, scale, scale);
	atlas = LoadTextureFromImage(im_atlas);

	texture_map = {
		{ UNSELECTED, Rectangle{.x = (10.0f * scale) + (1 * w) + (gap * scale * 1), .y = h, .width = w, .height = h} },
		{ EMPTY,      Rectangle{.x = (10.0f * scale) + (2 * w) + (gap * scale * 2), .y = h, .width = w, .height = h} },
		{ ONE,        Rectangle{.x = (10.0f * scale) + (3 * w) + (gap * scale * 3), .y = h, .width = w, .height = h} },
		{ TWO,        Rectangle{.x = (10.0f * scale) + (4 * w) + (gap * scale * 4), .y = h, .width = w, .height = h} },
		{ THREE,      Rectangle{.x = (10.0f * scale) + (5 * w) + (gap * scale * 5), .y = h, .width = w, .height = h} },
		{ FOUR,       Rectangle{.x = (10.0f * scale) + (6 * w) + (gap * scale * 6), .y = h, .width = w, .height = h} },
		{ FIVE,       Rectangle{.x = (10.0f * scale) + (7 * w) + (gap * scale * 7), .y = h, .width = w, .height = h} },
		{ SIX,        Rectangle{.x = (10.0f * scale) + (8 * w) + (gap * scale * 8), .y = h, .width = w, .height = h} },
		{ SEVEN,      Rectangle{.x = (10.0f * scale) + (9 * w) + (gap * scale * 9), .y = h, .width = w, .height = h} },
		{ EIGHT,      Rectangle{.x = (10.0f * scale) + (10 * w) + (gap * scale * 10), .y = h, .width = w, .height = h} },
		{ BOMB,       Rectangle{.x = 0, .y = 0, .width = w, .height = h} },
	};

	UnloadImage(im_atlas);
}

TextureHandler::~TextureHandler()
{
	UnloadTexture(atlas);
}

void TextureHandler::draw_texture(TextureType type, Vector2 pos)
{
	Rectangle src = texture_map.at(type);
	Rectangle dest = Rectangle{ pos.x, pos.y, width, height };
	DrawTexturePro(atlas, src, dest, Vector2{}, 0.f, WHITE);
}
