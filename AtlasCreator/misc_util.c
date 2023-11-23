#include "misc_util.h"
#include "mytypes.h"
#include "logging.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>

char* string_copy(char* string)
{
	if (string == NULL) return;
	usize len = strlen(string) + 1;
	char* copy = calloc(len, sizeof(char));
	if (copy == NULL) 
	{
		LOG_FATAL("string_copy failed");
	}
	for (i32 i = 0; i < len; i++) {
		copy[i] = string[i];
	}

	return copy;
}
