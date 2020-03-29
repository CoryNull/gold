#include "engine.hpp"
#include "var.hpp"

using namespace red;

int main() {
	engine main = engine();
	main("start");
	main("destroy");

	return 0;
}