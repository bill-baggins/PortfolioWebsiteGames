#ifndef GLOBALS_H
#define GLOBALS_H

#include "mytypes.h"


constexpr i32 SCREEN_WIDTH(1024);
constexpr i32 SCREEN_HEIGHT(768);

constexpr i32 MAX_WIDTH(SCREEN_WIDTH / 2);
constexpr i32 MAX_HEIGHT(SCREEN_HEIGHT / 2);

constexpr i32 PIXEL_WIDTH(SCREEN_WIDTH / MAX_WIDTH);
constexpr i32 PIXEL_HEIGHT(SCREEN_HEIGHT / MAX_HEIGHT);

constexpr i32 MIN_BLOB_RADIUS(1);
constexpr i32 MAX_BLOB_RADIUS(30);

constexpr f32 GRAVITY(4.805);
constexpr f32 TERM_VEL(12.14);

#endif
