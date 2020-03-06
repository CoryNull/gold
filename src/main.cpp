#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.hpp"
#include "engine.hpp"

using namespace red;

int main() {
	init();

	engine main = engine();
	main.callMethod("start");
	main.callMethod("destroy");

	cleanUp();
	return 0;
}