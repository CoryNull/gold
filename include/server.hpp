#pragma once

#include <App.h>
#include <HttpResponse.h>

#include "types.hpp"

#define serveArgs(args, req, res)          \
	auto req = args[0].getObject<request>(); \
	auto res = args[1].getObject<response>();

namespace gold {
	struct request;
	struct response;
	struct server : public object {
	 protected:
		static object& getPrototype();

	 public:
		server();
		server(object config);

		var start(list args = {});
		var get(list args);
		var post(list args);
		var put(list args);
		var patch(list args);
		var del(list args);
		var options(list args);
		var setMountPoint(list args);
		var setErrorHandler(list args);
		var initialize(list args = {});
		var destroy(list args = {});
	};

	struct request : public object {
	 protected:
		static object& getPrototype();

	 public:
		var getAllHeaders(list args = {});

		var getHeader(list args);
		var getMethod(list args = {});
		var getParameter(list args);
		var getQuery(list args);
		var getUrl(list args);
		var getYield(list args);
		var setParameters(list args);
		var setYield(list args);

		bool isWWWFormURLEncoded();
		bool isJSON();

		request();
		request(uWS::HttpRequest& req);
	};

	struct response : public object {
	 protected:
		static object& getPrototype();

	 public:
		response();
		response(uWS::HttpResponse<false>& res);
		response(uWS::HttpResponse<true>& res);

		var writeContinue(list args = {});
		var writeStatus(list args);
		var writeHeader(list args);
		var end(list args = {});
		var tryEnd(list args = {});
		var write(list args);
		var getWriteOffset(list args = {});
		var hasResponded(list args = {});
		var cork(list args);
		var onWritable(list args);
		var onAborted(list args);
		var onData(list args);
	};
}  // namespace gold