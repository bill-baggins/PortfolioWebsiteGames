#ifndef GLOBALS_H
#define GLOBALS_H

#include "mytypes.h"


constexpr i32 SCREEN_WIDTH(1280);
constexpr i32 SCREEN_HEIGHT(960);

constexpr i32 MAX_WIDTH(SCREEN_WIDTH / 3);
constexpr i32 MAX_HEIGHT(SCREEN_HEIGHT / 3);

constexpr i32 PIXEL_WIDTH(SCREEN_WIDTH / MAX_WIDTH);
constexpr i32 PIXEL_HEIGHT(SCREEN_HEIGHT / MAX_HEIGHT);

constexpr i32 CHUNK_WIDTH(32);
constexpr i32 CHUNK_HEIGHT(32);

constexpr i32 WORLD_CHUNK_WIDTH(MAX_WIDTH / CHUNK_WIDTH);
constexpr i32 WORLD_CHUNK_HEIGHT(MAX_HEIGHT / CHUNK_HEIGHT);

constexpr i32 MIN_BLOB_RADIUS(4);
constexpr i32 MAX_BLOB_RADIUS(30);

constexpr f32 GRAVITY(3.6f);
constexpr f32 TERM_VEL(12.14f);

constexpr i32 MAX_THREADS(2);

#endif
