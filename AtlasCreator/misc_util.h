#ifndef MISC_UTIL_H
#define MISC_UTIL_H

#include "mytypes.h"
#include <stdbool.h>

char* string_copy(str_t str);
void Rectangle_fmt(str_t buffer, struct Rectangle rect);
bool Rectangle_equals(struct Rectangle a, struct Rectangle b);

#endif