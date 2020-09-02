
find_package(OpenSSL REQUIRED)

add_library(uSockets 
	STATIC
		uSockets/src/crypto/openssl.c
		uSockets/src/crypto/wolfssl.c
		uSockets/src/eventing/epoll_kqueue.c
		uSockets/src/eventing/gcd.c
		uSockets/src/eventing/libuv.c
		uSockets/src/bsd.c
		uSockets/src/context.c
		uSockets/src/loop.c
		uSockets/src/socket.c
)
target_compile_definitions( uSockets PUBLIC LIBUS_USE_OPENSSL=1)
target_compile_definitions( uSockets PUBLIC LIBUS_USE_LIBUV=1)

if(MSVC)
	target_compile_definitions(uSockets PUBLIC _WIN32=1)
endif(MSVC)

target_include_directories(
	uSockets
	PUBLIC
		${OPENSSL_INCLUDE_DIR}
		uSockets/src/
)

target_link_libraries (uSockets 
	PUBLIC
		${OPENSSL_LIBRARIES}
		uv_a
)