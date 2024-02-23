#include "texture_handler.h"

TextureHandler::TextureHandler() : 
	m_width(0),
	m_height(0),
	m_atlas(Texture{})
{}

TextureHandler::TextureHandler(float scale)
{
	using enum TextureType;

	m_width = 30 * scale;
	m_height = 30 * scale;

	float w = m_width;
	float h = m_height;
	float gap = 3 * scale;
	
	Image im_atlas = LoadImage("resources/minesweeper.png");
	ImageResize(&im_atlas, scale, scale);
	m_atlas = LoadTextureFromImage(im_atlas);

	m_texture_map = {
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
	UnloadTexture(m_atlas);
}

void TextureHandler::draw_texture(TextureType type, Vector2 pos)
{
	Rectangle src = m_texture_map.at(type);
	Rectangle dest = Rectangle{ pos.x, pos.y, m_width, m_height };
	DrawTexturePro(m_atlas, src, dest, Vector2{}, 0.f, WHITE);
}
