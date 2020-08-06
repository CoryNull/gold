#pragma once

#include <string>
#include <ruby.h>
#include "types.hpp"

namespace gold {
	using namespace std;
	class module : public object {
		static var val2var(VALUE val);
		public:
		static object& getPrototype();
		module();
		module(string path);

		var execute(list args = {});
		var initialize(list args = {});
		var destroy(list args = {});

		template<class P>
		var registerProto(list args);
	};
}

#include "module.inl"