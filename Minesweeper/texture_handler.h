#ifndef TEXTURE_HANDLER_H
#define TEXTURE_HANDLER_H

#include "raylib.h"
#include "texture_type.h"
#include <unordered_map>


class TextureHandler
{
public:
    explicit TextureHandler();
	explicit TextureHandler(float scale);
	~TextureHandler();

    void draw_texture(TextureType type, Vector2 pos);

private:
    float     m_width, m_height;
    Texture   m_atlas;

    std::unordered_map<TextureType, Rectangle> m_texture_map;
};

#endif

