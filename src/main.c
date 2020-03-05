#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engine.h"

int main() {
	redInit();

	redEngine* engine = redNewEngine();
	redCallMethod(engine, "start", 0);
	redCallMethod(engine, "destroy", 0);
	redFree(engine);

	redCleanUp();
	return 0;
}