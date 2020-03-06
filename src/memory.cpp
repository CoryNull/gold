#include "memory.hpp"
#include "object.hpp"
#include "array.hpp"
#include <stdlib.h>

namespace red {
	bool isCopyable(types type) {
		if (type == typeString)
			return true;
		if (sizeof(uint64_t) > sizeof(void*) && type == typeUInt64)
			return true;
		if (sizeof(int64_t) > sizeof(void*) && type == typeInt64)
			return true;
		if (sizeof(double) > sizeof(void*) && type == typeDouble)
			return true;
		return false;
	}

	typedef struct chainLink_t chainLink;
	struct chainLink_t {
		chainLink* last;
		void* data;
		types type;
		chainLink* next;
	};

	struct {
		AllocFunction alloc;
		ReallocFunction realloc;
		FreeFunction free;
		uint64_t dataSize;
		chainLink* dataStart;
		chainLink* dataEnd;
	} redInstance;

	void freechainLink(chainLink* link) {
		if (link != nullptr) {
			if (link->last != nullptr)
				link->last->next = link->next;
			if (link->next != nullptr)
				link->next->last = link->last;
			if (link->data != nullptr) {
				switch (link->type) {
					case typeObject:
						// destroyObject((object*)link->data);
						break;
					case typeArray:
						// destroyArray((array*)link->data);
						break;
					default:
						redInstance.free(link->data);
						break;
				}
			}
			redInstance.free(link);
		}
	}

	void setMemhook(AllocFunction a, ReallocFunction r, FreeFunction f) {
		redInstance.alloc = a;
		redInstance.realloc = r;
		redInstance.free = f;
	}

	void init() {
		if (redInstance.alloc == nullptr)
			redInstance.alloc = std::malloc;
		if (redInstance.realloc == nullptr)
			redInstance.realloc = std::realloc;
		if (redInstance.free == nullptr)
			redInstance.free = std::free;
		redInstance.dataSize = 0;
		redInstance.dataEnd = nullptr;
		redInstance.dataStart = nullptr;
	}

	void cleanUp() {
		// destroy from the end of the chain
		if (redInstance.dataStart != nullptr) {
			uint64_t count = 0;
			chainLink* current = redInstance.dataStart;
			while (current != nullptr) {
				count += 1;
				chainLink* next = current->next;
				freechainLink(current);
				current = next;
			}
			if (count != redInstance.dataSize)
				printf("[Backend] Didn't clean up correctly!\n");
		}
	}

	void* alloc(uint64_t size, types type) {
		if (size == 0)
			return nullptr;
		chainLink* link = nullptr;
		chainLink* linkEnd = redInstance.dataEnd;
		link = (chainLink*)redInstance.alloc(sizeof(chainLink));
		memset(link, 0, sizeof(chainLink));
		link->data = redInstance.alloc(size);
		memset(link->data, 0, size);
		link->type = type;
		if (linkEnd) {
			link->last = linkEnd;
			linkEnd->next = link;
		}
		if (!redInstance.dataStart)
			redInstance.dataStart = link;
		redInstance.dataEnd = link;
		redInstance.dataSize += 1;
		return link->data;
	}

	void* realloc(void* ptr, uint64_t size, types type) {
		if (size == 0)
			return nullptr;
		if (ptr != nullptr) {
			chainLink* current = redInstance.dataEnd;
			while (current != nullptr) {
				if (current->data == ptr) {
					current->data = redInstance.realloc(ptr, size);
					current->type = type;
					return current->data;
				}
				current = current->last;
			}
		}
		return alloc(size, type);
	}

	void free(void* ptr) {
		chainLink* current = redInstance.dataEnd;
		while (current != nullptr) {
			if (current->data == ptr) {
				if (current->last != nullptr)
					current->last->next = current->next;
				if (current->next != nullptr)
					current->next->last = current->last;
				redInstance.free(current->data);
				redInstance.free(current);
				redInstance.dataSize -= 1;
				return;
			}
			current = current->last;
		}
	}
}  // namespace red