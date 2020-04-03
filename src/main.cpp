#include <iostream>
#include <thread>

#include "component.hpp"
#include "engine.hpp"
#include "entity.hpp"
#include "object.hpp"
#include "var.hpp"
#include "sprite.hpp"

using namespace red;

int main() {
	engine main = engine();

	auto entity00 = entity({{"name", "entity00"}});
	auto callback = method([](object&, var) {
		cout << "hello world from " << this_thread::get_id()
				 << endl;
		return var();
	});
	entity00 += component({{"update", callback}});
	entity00 += sprite();
	main += entity00;

	main("start");

	return 0;
}