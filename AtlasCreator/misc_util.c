#include "misc_util.h"
#include "mytypes.h"
#include "logging.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

char* string_copy(str_t str)
{
	if (str == NULL) return NULL;

	usize len = strlen(str) + 1;
	char* copy = calloc(len, sizeof(char));
	if (copy == NULL) 
	{
		LOG_FATAL("string_copy failed");
	}
	for (i32 i = 0; i < len; i++) {
		copy[i] = str[i];
	}

	return copy;
}
