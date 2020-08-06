#pragma once

#include <cstddef>

#include "types.hpp"

namespace gold {
	template <class P> var module::registerProto(list args) {
		auto super = object();
		auto proto = P::getPrototype();
		auto name = string();
		if (args.size() >= 1) {
			auto it = args.begin();
			name = (it++)->getString();
			if (args.size() >= 2) super = (it++)->getObject();
		}
		if (name.size() > 0 && proto) {
			auto supVal = VALUE(0);
			if (super) supVal = super.getUInt64("^");
			auto klass = rb_define_class(name.c_str(), supVal);
			auto newKlass = std::function(
				[](int argc, VALUE* argv, VALUE self) -> VALUE {
					auto inst = obj();
					auto proto = P::getPrototype();
					inst.setParent(proto);
				});
			auto klassType = rb_data_type_t{
				.wrap_struct_name = name.c_str(),
				.function =
					{
						.dmark = nullptr,
						.dfree = free,
						.dsize = []() -> size_t { return sizeof(P); },
					},
				.data = nullptr,
				.flags = RUBY_TYPED_FREE_IMMEDIATELY,
			};
			auto initKlass =
				[](int argc, VALUE* argv, VALUE self) -> VALUE {
				auto data = object();
				TypedData_Get_Struct(self, P, &klassType, &data);
				auto args = list();
				for (auto i = 0; i < argc; ++i) {
					auto arg = argv[i];
					auto argT = type(arg);
					if (argT == RUBY_T_STRING) {
						args.pushString(rb_string_value_cstr(arg));
					} else if (argT == RUBY_T_FLOAT) {
						args.pushDouble(rb_float_value(arg));
					} else if (argT == RUBY_T_FIXNUM) {
						args.pushInt64(rb_fix2long(arg));
					} else if (argT == RUBY_T_BIGNUM) {
						args.pushUInt64(rb_big2ulong(arg));
					} else if (argT == RUBY_T)
				}
				data.callMethod("initialize", args);
			};
			rb_define_singleton_method(klass, "new", newKlass, -1);
			proto.setUInt64("^", klass);

			for (auto it = proto.begin(); it != proto.end(); ++it) {
				auto key = it->first;
				auto value = it->second;
				if (value.isMethod()) {
					auto meth = value.getMethod();
					auto wrap = [](
												int argc, VALUE* argv,
												VALUE self) -> VALUE { return Qnil; };
					rb_define_method(klass, key.c_str(), wrap, -1);
				}
			}

			return klass;
		}
		return var();
	}
}  // namespace gold
