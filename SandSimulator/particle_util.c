#include "particle_funcs.h"
#include "mytypes.h"
#include <stdio.h>

static char* particle_types[] = {
	"AIR",
	"SAND",
	"WATER",
	"STONE"
};

bool is_inbounds(i32 x, i32 y)
{
	return x >= 0 && x < MAX_WIDTH && y >= 0 && y < MAX_HEIGHT;
}

bool is_inbounds_v(Vector2 pos)
{
	return pos.x >= 0 && pos.x < MAX_WIDTH && pos.y >= 0 && pos.y < MAX_HEIGHT;
}


void ParticleType_snprintf(char *buffer, usize size, char *fmt, ParticleType type)
{
	usize len = sizeof(particle_types) / sizeof(particle_types[0]);
	char* particle_string = "Unknown";

	if ((usize)type < len)
	{
		particle_string = particle_types[type];
	}
	snprintf(buffer, size, fmt, particle_string);
}



