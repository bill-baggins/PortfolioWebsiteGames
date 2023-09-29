#ifndef RAYLIB_HASHES_H
#define RAYLIB_HASHES_H

#include <unordered_map>

bool operator==(const struct Rectangle& a, const struct Rectangle& b);

template<>
struct std::hash<struct Rectangle> 
{
	std::size_t operator()(const struct Rectangle& rect) const;
};


#endif
