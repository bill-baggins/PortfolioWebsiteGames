#ifndef TEXTURE_HANDLER_H
#define TEXTURE_HANDLER_H

#include "raylib.h"
#include "texture_type.h"
#include <unordered_map>


class TextureHandler
{
public:
	TextureHandler(float scale);
	~TextureHandler();

    void draw_texture(TextureType type, Vector2 pos);

private:
    float     width, height;
    Texture   atlas;

    std::unordered_map<TextureType, Rectangle> texture_map;
};

#endif

