#include "entry_point.h"
#include <memory>

#include <vector>
#include <iostream>


int main()
{
	auto game = std::make_unique<EntryPoint>();
	game->run();

	return 0;
}