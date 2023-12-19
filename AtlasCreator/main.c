#include "game.h"

/*
	TODO: Need to find a way to resolve having different
	frame sizes for each frame of an animation.
	
	Maybe just consider working on writing to a JSON
	file first before delving into this?
*/

int main(int argc, char* argv[]) 
{
	Game* game = Game_new();
	Game_run(game);

	return 0;
}


/**
 * Example of writing to a JSON file from cJSON.
 * 
 * Rectangle rectangles[] = {
		{1, 2, 3, 4},
		{2, 3, 4, 5},
		{3, 4, 5, 6},
		{4, 5, 6, 7},
		{5, 6, 7, 8},
		{7, 8, 9, 10}
	};

	cJSON* object = cJSON_CreateObject();
	cJSON* arr_of_arr = cJSON_AddArrayToObject(object, "arr");

	for (int i = 0; i < 6; i++) 
	{
		cJSON* arr = cJSON_CreateArray();
		float* rect_arr = (float*)&rectangles[i];

		printf("adding ");
		for (int j = 0; j < 4; j++) 
		{
			printf("%.0f ", rect_arr[j]);
			cJSON* num = cJSON_CreateNumber(rect_arr[j]);
			cJSON_AddItemToArray(arr, num);
		}
		puts("");

		cJSON_AddItemToArray(arr_of_arr, arr);
	}

	char* str = cJSON_Print(object);

	FILE* file = fopen("test_obj.json", "w+");
	if (file == NULL) {
		LOG_FATAL("pp");
	}

	fputs(str, file);

	fclose(file);

	puts("wrote to test_obj.json");

	cJSON_Delete(object);.
 */





