
find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)

add_library(
	uWebSockets
	OBJECT
		uWebSockets/src/f2/function2.hpp
		uWebSockets/src/App.h
		uWebSockets/src/AsyncSocket.h
		uWebSockets/src/AsyncSocketData.h
		uWebSockets/src/BloomFilter.h
		uWebSockets/src/HttpContext.h
		uWebSockets/src/HttpContextData.h
		uWebSockets/src/HttpParser.h
		uWebSockets/src/HttpResponse.h
		uWebSockets/src/HttpResponseData.h
		uWebSockets/src/HttpRouter.h
		uWebSockets/src/Loop.h
		uWebSockets/src/LoopData.h
		uWebSockets/src/PerMessageDeflate.h
		uWebSockets/src/TopicTree.h
		uWebSockets/src/Utilities.h
		uWebSockets/src/WebSocket.h
		uWebSockets/src/WebSocketContext.h
		uWebSockets/src/WebSocketContextData.h
		uWebSockets/src/WebSocketData.h
		uWebSockets/src/WebSocketExtensions.h
		uWebSockets/src/WebSocketHandshake.h
		uWebSockets/src/WebSocketProtocol.h
)

target_include_directories(
	uWebSockets
	PUBLIC
		uWebSockets/src/
)

target_link_libraries (uWebSockets 
	PUBLIC
		uSockets
		ZLIB::ZLIB
		${OPENSSL_LIBRARIES}
		stdc++fs
)