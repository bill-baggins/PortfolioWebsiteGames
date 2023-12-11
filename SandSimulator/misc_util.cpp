#include "misc_util.h"
#include "mytypes.h"
#include "logging.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <math.h>
#include <assert.h>

#include "raylib.h"

char* string_copy(str_t str)
{
	if (str == NULL) return NULL;

	usize len = strlen(str) + 1;
	char* copy = (char*)calloc(len, sizeof(char));
	if (copy == NULL)
	{
		LOG_FATAL("string_copy failed");
	}

	for (i32 i = 0; i < len; i++)
	{
		copy[i] = str[i];
	}

	return copy;
}

void Rectangle_fmt(char* buffer, Rectangle rect)
{
	int matched = sscanf_s(buffer, "<%f,%f,%f,%f>", &rect.x, &rect.y, &rect.width, &rect.height);
	assert(matched == 4);
}
