#pragma once

#include "object.hpp"

namespace red
{
	class backend : public object {
		protected:
		public:

		backend();
	};

	class vulkanBackend : public backend {
		protected:
		public:

		vulkanBackend();
		
		static bool isSupported();
	};

	class openGLBackend : public backend {
		protected:
		public:

		openGLBackend();
		
	};
} // namespace red
