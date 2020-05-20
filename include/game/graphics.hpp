#pragma once

#include "types.hpp"
#include "window.hpp"

namespace gold {
	struct gfxBackend : public object {
	 protected:
		static object& getPrototype();

	 public:
		gfxBackend();
		gfxBackend(object config);

		var destroy();
		var initialize(list args = {});
		var preFrame();
		var renderFrame();
		var getConfig();
	};

	struct frameBuffer : public object {
	 protected:
	 	friend gfxBackend;
		static object& getPrototype();
		static map<string, frameBuffer> cache;

	 public:
		enum backbufferRatio {
			Equal,      // Equal to backbuffer.
			Half,       // One half size of backbuffer.
			Quarter,    // One quarter size of backbuffer.
			Eighth,     // One eighth size of backbuffer.
			Sixteenth,  // One sixteenth size of backbuffer.
			Double,     // Double size of backbuffer.
		};

		frameBuffer();
		frameBuffer(object config);

		void setName(string name);

		var getTexture(uint8_t attachment = 0);

		void setViewFrameBuffer(uint16_t viewId);

		void requestScreenShot(string path);
		void destroy();
	};

	struct occlusionQuery : public object {
	 protected:
	 	friend gfxBackend;
		static object& getPrototype();
		static map<string, occlusionQuery> cache;

	 public:
		enum queryResult {
			Invisible,  // Query failed test.
			Visible,    // Query passed test.
			NoResult,   // Query result is not available yet.
		};
		occlusionQuery();
		occlusionQuery(object config);

		queryResult getResult(int32_t* result = nullptr);
		void setCondition(bool visible);
		void destroy();
	};

	struct indirectBuffer : public object {
	 protected:
	 	friend gfxBackend;
		static object& getPrototype();
		static map<string, indirectBuffer> cache;

	 public:
		indirectBuffer();
		indirectBuffer(object config);
		void destroy();
	};

	struct shaderObject : public object {
	 protected:
	 	friend gfxBackend;
		static object& getPrototype();
		static map<string, shaderObject> cache;

	 public:
		shaderObject();
		shaderObject(object config);

		var getAllUniforms();
		void destroy();
	};

	enum uniformType {
		Sampler,  // Sampler.
		End,      // Reserved, do not use.
		Vec4,     // 4 floats vector.
		Mat3,     // 3x3 matrix.
		Mat4,     // 4x4 matrix.
	};

	struct gpuTexture;
	struct shaderProgram : public object {
	 protected:
	 	friend gfxBackend;
		static object& getPrototype();
		static map<string, shaderProgram> cache;
		static map<string, object> uniforms;

	 public:
		static shaderProgram findInCache(string name);

		shaderProgram();
		shaderProgram(object config);

		static bool createUniform(
			string name, uniformType t, uint16_t num = 1U);

		static bool setUniform(
			string name, const void* value, uint16_t num = UINT16_MAX);

		static void bindTexture(
			string sampler, uint8_t stage, gpuTexture tex);

		void setState(object state);
		void defaultState();

		void setTransform(var& mtx);

		void setStencil(object state);
		void defaultStencil();

		void setDiscard(string state);
		void defaultDiscard();

		void submit(uint8_t viewId, uint32_t depth = 0);
		void submit(
			uint8_t viewId, occlusionQuery query, uint32_t depth = 0);
		void submit(
			uint8_t viewId, indirectBuffer buffer, uint16_t start = 0,
			uint16_t num = 1, uint32_t depth = 0);

		void dispatch(
			uint8_t viewId, uint32_t numX = 1, uint32_t numY = 1,
			uint32_t numZ = 1);
		void dispatch(
			uint8_t viewId, indirectBuffer buffer, uint16_t start = 0,
			uint16_t num = 1);
		void destroy();
	};

	struct gpuTexture : public object {
	 protected:
	 	friend gfxBackend;
		static object& getPrototype();
		static map<string, gpuTexture> cache;

	 public:
		enum textureFormat {
			/* clang-format off */
			/* Compressed Formats Below */
			BC1,			// DXT1 R5G6B5A1
			BC2,			// DXT3 R5G6B5A4
			BC3,			// DXT5 R5G6B5A8
			BC4,			// LATC1/ATI1 R8
			BC5,			// LATC2/ATI2 RG8
			BC6H,			// BC6H RGB16F
			BC7,			// BC7 RGB 4-7 bits per color channel, 0-8 bits alpha
			ETC1,			// ETC1 RGB8
			ETC2,			// ETC2 RGB8
			ETC2A,		// ETC2 RGBA8
			ETC2A1,		// ETC2 RGB8A1
			PTC12,		// PVRTC1 RGB 2BPP
			PTC14,		// PVRTC1 RGB 4BPP
			PTC12A,		// PVRTC1 RGBA 2BPP
			PTC14A,		// PVRTC1 RGBA 4BPP
			PTC22,		// PVRTC2 RGBA 2BPP
			PTC24,		// PVRTC2 RGBA 4BPP
			ATC,			// ATC RGB 4BPP
			ATCE,			// ATCE RGBA 8 BPP explicit alpha
			ATCI,			// ATCI RGBA 8 BPP interpolated alpha
			ASTC4x4,	// ASTC 4x4 8.0 BPP
			ASTC5x5,	// ASTC 5x5 5.12 BPP
			ASTC6x6,	// ASTC 6x6 3.56 BPP
			ASTC8x5,	// ASTC 8x5 3.20 BPP
			ASTC8x6,	// ASTC 8x6 2.67 BPP
			ASTC10x5,	// ASTC 10x5 2.56 BPP
			/* Standard Formats Below */
			Unknown,R1,A8,R8,R8I,R8U,R8S,
			R16,R16I,R16U,R16F,R16S,R32I,R32U,R32F,RG8,RG8I,RG8U,RG8S,
			RG16,RG16I,RG16U,RG16F,RG16S,RG32I,RG32U,RG32F,RGB8,RGB8I,
			RGB8U,RGB8S,RGB9E5F,BGRA8,RGBA8,RGBA8I,RGBA8U,RGBA8S,RGBA16,
			RGBA16I,RGBA16U,RGBA16F,RGBA16S,RGBA32I,RGBA32U,RGBA32F,
			R5G6B5,RGBA4,RGB5A1,RGB10A2,RG11B10F,UnknownDepth,D16,D24,
			D24S8,D32,D16F,D24F,D32F,D0S8
			/* clang-format on */
		};
		enum accessType {
			Read,       // Read
			Write,      // Write
			ReadWrite,  // Read and write
		};
		gpuTexture();
		gpuTexture(object config);

		void update(object data);

		void setImage(
			uint8_t stage, uint8_t mip, accessType t,
			textureFormat f = (textureFormat)85);

		void blit(
			uint8_t viewId, var dstP, gpuTexture src, var srcP,
			var size = vec2u16(UINT16_MAX, UINT16_MAX));
		void blit(
			uint8_t viewId, uint8_t dstMip, var dstP, gpuTexture src,
			uint8_t srcMip = 0, var srcP = vec3u16(0, 0, 0),
			var size = vec3u16(UINT16_MAX, UINT16_MAX, UINT16_MAX));

		uint32_t readTexture(void* data, uint8_t mip = 0);
		void* getDirectAccessPtr();
		void setName(string name);
		void destroy();
	};

	struct vertexLayout : public object {
	 protected:
	 	friend gfxBackend;
		static object& getPrototype();
		static map<string, vertexLayout> cache;

	 public:
		static vertexLayout findInCache(string name);

		enum attrib {
			aPosition,   // shader: a_position
			aNormal,     // shader: a_normal
			aTangent,    // shader: a_tangent
			aBitangent,  // shader: a_bitangent
			aColor0,     // shader: a_color0
			aColor1,     // shader: a_color1
			aColor2,     // shader: a_color2
			aColor3,     // shader: a_color3
			aIndices,    // shader: a_indices
			aWeight,     // shader: a_weight
			aTexCoord0,  // shader: a_texcoord0
			aTexCoord1,  // shader: a_texcoord1
			aTexCoord2,  // shader: a_texcoord2
			aTexCoord3,  // shader: a_texcoord3
			aTexCoord4,  // shader: a_texcoord4
			aTexCoord5,  // shader: a_texcoord5
			aTexCoord6,  // shader: a_texcoord6
			aTexCoord7,  // shader: a_texcoord7
		};
		enum attribType {
			Uint8,   // Uint8
			Uint10,  // Uint10, depends on availability
			Int16,   // Int16
			Half,    // Half, depends on availability
			Float,   // Float
		};

		vertexLayout();
		vertexLayout(object config);

		vertexLayout& begin();
		vertexLayout& add(
			attrib att, attribType t, uint8_t count,
			bool norm = false, bool isInt = false);
		vertexLayout& end();
		void destroy();
	};

	enum BufferType {
		nullBufferType = 0,
		standardBufferType = 1,
		dynamicBufferType = 2,
		transientBufferType = 3,
	};

	struct vertexBuffer : public object {
	 protected:
	 	friend gfxBackend;
		static object& getPrototype();

	 public:
		vertexBuffer();
		vertexBuffer(object config);

		void set(uint8_t stream);
		void set(
			uint8_t stream, uint32_t start, uint32_t num,
			vertexLayout layout = vertexLayout());

		void update(
			binary data, uint64_t start = 0,
			uint64_t end = UINT64_MAX);

		void destroy();
	};

	struct indexBuffer : public object {
	 protected:
	 	friend gfxBackend;
		static object& getPrototype();

	 public:
		indexBuffer();
		indexBuffer(object config);

		void set();
		void set(uint32_t start, uint32_t num);

		void update(
			binary data, uint64_t start = 0,
			uint64_t end = UINT64_MAX);

		void destroy();
	};

}  // namespace gold
