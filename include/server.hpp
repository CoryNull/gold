#pragma once

#include <App.h>
#include <HttpResponse.h>

#include "object.hpp"

namespace gold {
	class request;
	class response;
	class server : public object {
	 protected:
		static object proto;

	 public:
		server();
		server(object config);

		var start(varList args = {});
		var get(varList args);
		var post(varList args);
		var put(varList args);
		var patch(varList args);
		var del(varList args);
		var options(varList args);
		var setMountPoint(varList args);
		var setErrorHandler(varList args);
		var initialize(varList args = {});
		var destroy(varList args = {});
	};

	class request : public object {
	 protected:
		static object proto;

	 public:

		var getAllHeaders(varList args = {});

		var getHeader(varList args);
		var getMethod(varList args = {});
		var getParameter(varList args);
		var getQuery(varList args);
		var getUrl(varList args);
		var getYield(varList args);
		var setParameters(varList args);
		var setYield(varList args);

		bool isWWWFormURLEncoded();
		bool isJSON();

		request(uWS::HttpRequest& req);
	};

	class response : public object {
	 protected:
		static object proto;

	 public:
		response(uWS::HttpResponse<false>& res);
		response(uWS::HttpResponse<true>& res);

		var writeContinue(varList args = {});
		var writeStatus(varList args);
		var writeHeader(varList args);
		var end(varList args = {});
		var tryEnd(varList args = {});
		var write(varList args);
		var getWriteOffset(varList args = {});
		var hasResponded(varList args = {});
		var cork(varList args);
		var onWritable(varList args);
		var onAborted(varList args);
		var onData(varList args);
	};
}  // namespace gold