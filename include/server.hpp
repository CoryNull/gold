#pragma once

#include "object.hpp"

// Forward declares
namespace httplib {
	class Request;
	class Response;
}  // namespace httplib

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
		var removeMountPoint(varList args);
		var initialize(varList args = {});
		var destroy(varList args = {});
	};

	class request : public object {
	 protected:
		static object proto;

	 public:
		var hasHeader(varList args);
		var getHeaderValue(varList args);
		var getHeaderValueCount(varList args);

		var hasParam(varList args);
		var getParamValue(varList args);
		var getParamValueCount(varList args);

		var isMultipartFormData(varList args);

		var hasFile(varList args);
		var getFileValue(varList args);

		var getMatches(varList args = {});

		request(const httplib::Request& req);
	};

	class response : public object {
	 protected:
		static object proto;

	 public:
		response(httplib::Response& res);

		var hasHeader(varList args);
		var getHeaderValue(varList args);
		var getHeaderValueCount(varList args);
		var setHeader(varList args);

		var setRedirect(varList args);
		var setContent(varList args);

		var setContentProvider(varList args);
		var setChunkedContentProvider(varList args);
	};
}  // namespace gold