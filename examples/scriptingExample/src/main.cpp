#include <iostream>
#include <module.hpp>
#include <thread>

using namespace gold;

int main() {
	auto mod = module("./app/main.rb");
	mod.execute();

	return 0;
}