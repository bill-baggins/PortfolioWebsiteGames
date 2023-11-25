#ifndef GLOBALS_H
#define GLOBALS_H

// Needed for the structures inside of std_ds.h to work.
#define STB_DS_IMPLEMENTATION

const static int SCREEN_WIDTH = 1400;
const static int SCREEN_HEIGHT = 900;
const static char* title = "Atlas Addresser";

#define IMAGE_VIEWPORT_WIDTH 800
#define IMAGE_VIEWPORT_HEIGHT 900

#define GUI_VIEWPORT_WIDTH 600
#define GUI_VIEWPORT_HEIGHT 900

#define GROUP_NAME_MAX 128

#define deeparrfree(arr, func) \
	for (i32 i = 0; i < arrlen((arr)); i++) (func)((arr)[i]); \
	arrfree((arr));

#endif