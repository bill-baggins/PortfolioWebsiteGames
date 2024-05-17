#include "raylib_hashes.h"
#include "raylib.h"


bool operator==(const struct Rectangle& a, const struct Rectangle& b)
{
	return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
}

inline std::size_t std::hash<Rectangle>::operator()(const Rectangle& rect) const
{
	return std::hash<int>{}(
		(int)rect.x ^ ((int)rect.y << 1) ^ ((int)rect.width << 2) ^ ((int)rect.height << 3)
	);
}