#ifndef GLOBALS_H
#define GLOBALS_H

const static int SCREEN_WIDTH = 1400;
const static int SCREEN_HEIGHT = 900;
const static char* title = "Atlas Addresser";

#define IMAGE_VIEWPORT_WIDTH 800
#define IMAGE_VIEWPORT_HEIGHT 900

#define GUI_VIEWPORT_WIDTH 600
#define GUI_VIEWPORT_HEIGHT 900

#define GROUP_NAME_MAX_LENGTH 64
#define ERROR_MAX_LENGTH 128

#define deeparrfree(arr, func) \
	for (i32 i = 0; i < arrlen((arr)); i++) (func)((arr)[i]); \
	arrfree((arr));


#endif