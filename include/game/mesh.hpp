#pragma once

#include "file.hpp"

namespace gold {
	using namespace std;
	struct mesh : public file {
	 protected:
		static object& getPrototype();
		static void parseGLTFBuffer(
			string type,
			uint32_t scalarType,
			uint64_t offset,
			uint64_t length,
			void* data,
			list& parsed);

	 public:
		mesh();
		mesh(path meshFilePath);
		mesh(file meshFile);

		var getNode(string name);
		var getPrimitiveCount(string nName);
		var getVertexLayoutHandle(string nName, uint64_t primitive = 0);
		var getVertexBufferHandle(string nName, uint64_t primitive = 0);
		var getIndexBufferHandle(string nName, uint64_t primitive = 0);
		var getTrianglesFromMesh(string nName);
		var getMaterial(string nName);
		var getMaterial(uint64_t id);
		var getMaterialFromPrimitive(string nName, uint64_t primitive);

		var destroy();
	};
}  // namespace gold
