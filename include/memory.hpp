#pragma once

#include "types.hpp"

namespace red {
	/* <Memory> */

	typedef void* (*AllocFunction)(uint64_t size);
	typedef void* (*ReallocFunction)(void* src, uint64_t size);
	typedef void (*FreeFunction)(void* ptr);

	void setMemhook(AllocFunction, ReallocFunction, FreeFunction);
	void init();
	void cleanUp();

	void* alloc(uint64_t size, types type);
	void* realloc(void* ptr, uint64_t size, types type);
	void free(void* ptr);

	/* </Memory> */

}  // namespace red