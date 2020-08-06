#include "module.hpp"

#include <ruby.h>

namespace gold {

	var module::val2var(VALUE val) {
		auto argT = TYPE(val);
		if (argT == RUBY_T_STRING) {
			return rb_string_value_cstr(&val);
		} else if (argT == RUBY_T_FLOAT) {
			return rb_float_value(val);
		} else if (argT == RUBY_T_FIXNUM) {
			return rb_fix2long(val);
		} else if (argT == RUBY_T_BIGNUM) {
			return rb_big2ulong(val);
		} else if (argT == RUBY_T_TRUE) {
			return var(true);
		} else if (argT == RUBY_T_FALSE) {
			return var(false);
		} else if (argT == RUBY_T_OBJECT) {
			auto ret = obj();
			auto size = rb_ivar_count(val);
			for (st_index_t i = 0; i < size; ++i) {
				auto value = rb_ivar_get(val, i);
				auto key = rb_id2name(i);
				if (key != nullptr) {
					auto value = rb_iv_get(val, key);
					ret.setVar(key, val2var(value));
				} else {
					break;
				}
			}
			return ret;
		} else if(argT == RUBY_T_ARRAY) {
			auto ret = list();
			auto size = rb_array_len(val);
			for (long i = 0; i < size; ++i) {
				auto item = rb_ary_entry(val, i);
			}
		}
		return var();
	}

	object& module::getPrototype() {
		static auto proto = obj{

		};
		return proto;
	}

	module::module() {}

	module::module(string path) {
		setParent(module::getPrototype());
		setString("path", path);
		initialize();
	}

	var module::execute(list args) {
		string filePath = getString("path");
		char* options[] = {"-v", (char*)filePath.c_str()};
		void* node = ruby_options(2, options);

		int state;
		if (ruby_executable_node(node, &state)) {
			state = ruby_exec_node(node);
		}

		if (state) {
			auto exception = rb_errinfo();
			rb_set_errinfo(Qnil);
		}
		return var();
	}

	var module::initialize(list args) {
		ruby_init();
		return var();
	}

	var module::destroy(list args) {
		ruby_cleanup(0);
		return var();
	}

}  // namespace gold