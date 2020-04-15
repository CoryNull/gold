#include "server.hpp"

#include <functional>

#include "array.hpp"
#include "httplib.h"

namespace gold {
	using namespace std;
	using namespace httplib;

	object server::proto = object({
		{"host", "localhost"},
		{"port", 8080},
		{"start", method(&server::start)},
		{"get", method(&server::get)},
		{"post", method(&server::post)},
		{"put", method(&server::put)},
		{"patch", method(&server::patch)},
		{"del", method(&server::del)},
		{"options", method(&server::options)},
		{"setMountPoint", method(&server::setMountPoint)},
		{"removeMountPoint", method(&server::removeMountPoint)},
		{"initialize", method(&server::initialize)},
		{"destroy", method(&server::destroy)},
	});

	object response::proto = object({
		{"hasHeader", method(&response::hasHeader)},
		{"getHeaderValue", method(&response::getHeaderValue)},
		{"getHeaderValueCount",
		 method(&response::getHeaderValueCount)},
		{"setHeader", method(&response::setHeader)},

		{"setRedirect", method(&response::setRedirect)},
		{"setContent", method(&response::setContent)},
		{"setContentProvider",
		 method(&response::setContentProvider)},
		{"setChunkedContentProvider",
		 method(&response::setChunkedContentProvider)},
	});

	object request::proto = object({
		{"hasHeader", method(&request::hasHeader)},
		{"getHeaderValue", method(&request::getHeaderValue)},
		{"getHeaderValueCount",
		 method(&request::getHeaderValueCount)},
		{"hasParam", method(&request::hasParam)},
		{"getParamValue", method(&request::getParamValue)},
		{"getParamValueCount",
		 method(&request::getParamValueCount)},
		{"isMultipartFormData",
		 method(&request::isMultipartFormData)},
		{"hasFile", method(&request::hasFile)},
		{"getFileValue", method(&request::getFileValue)},
		{"getMatches", method(&request::getMatches)},
	});

	var server::start(varList args) {
		auto host = getString("host");
		auto port = getInt32("port");
		auto handle = (Server*)getPtr("handle");
		if (handle) return handle->listen(host.c_str(), port);
		return var();
	}

	var server::get(varList args) {
		auto handle = (Server*)getPtr("handle");
		if (!handle) return genericError("server handle null");
		auto pattern = args[0].getString();
		auto func = args[1].getFunction();
		if (!func)
			return genericError("missing callback");
		else
			handle->Get(
				pattern.c_str(),
				[func](const Request& req, Response& res) {
					func({request(req), response(res)});
				});
		return var();
	}

	var server::post(varList args) {
		auto handle = (Server*)getPtr("handle");
		if (!handle) return genericError("server handle null");
		auto pattern = args[0].getString();
		auto func = args[1].getFunction();
		if (!func)
			return genericError("missing callback");
		else
			handle->Post(
				pattern.c_str(),
				[func](const Request& req, Response& res) {
					func({request(req), response(res)});
				});
		return var();
	}

	var server::put(varList args) {
		auto handle = (Server*)getPtr("handle");
		if (!handle) return genericError("server handle null");
		auto pattern = args[0].getString();
		auto func = args[1].getFunction();
		if (!func)
			return genericError("missing callback");
		else
			handle->Put(
				pattern.c_str(),
				[func](const Request& req, Response& res) {
					func({request(req), response(res)});
				});
		return var();
	}

	var server::patch(varList args) {
		auto handle = (Server*)getPtr("handle");
		if (!handle) return genericError("server handle null");
		auto pattern = args[0].getString();
		auto func = args[1].getFunction();
		if (!func)
			return genericError("missing callback");
		else
			handle->Patch(
				pattern.c_str(),
				[func](const Request& req, Response& res) {
					func({request(req), response(res)});
				});
		return var();
	}

	var server::del(varList args) {
		auto handle = (Server*)getPtr("handle");
		if (!handle) return genericError("server handle null");
		auto pattern = args[0].getString();
		auto func = args[1].getFunction();
		if (!func)
			return genericError("missing callback");
		else
			handle->Delete(
				pattern.c_str(),
				[func](const Request& req, Response& res) {
					func({request(req), response(res)});
				});
		return var();
	}

	var server::options(varList args) {
		auto handle = (Server*)getPtr("handle");
		if (!handle) return genericError("server handle null");
		auto pattern = args[0].getString();
		auto func = args[1].getFunction();
		if (!func)
			return genericError("missing callback");
		else
			handle->Options(
				pattern.c_str(),
				[func](const Request& req, Response& res) {
					func({request(req), response(res)});
				});
		return var();
	}

	var server::setMountPoint(varList args) {
		auto handle = (Server*)getPtr("handle");
		auto point = args[0].getString();
		auto dir = args[1].getString();
		return handle->set_mount_point(point.c_str(), dir.c_str());
	}

	var server::removeMountPoint(varList args) {
		auto handle = (Server*)getPtr("handle");
		auto point = args[0].getString();
		return handle->remove_mount_point(point.c_str());
	}

	var server::initialize(varList args) {
		setPtr("handle", new Server());
		return var();
	}

	var server::destroy(varList args) {
		auto handle = (Server*)getPtr("handle");
		if (handle) delete handle;
		return var();
	}

	server::server() : object(&proto) { initialize({}); }

	server::server(object config) : object(config, &proto) {
		initialize({});
	}

	response::response(httplib::Response& r) : object(&proto) {
		setPtr("handle", &r);
	}

	var response::hasHeader(varList args) {
		auto name = args[0].getString();
		auto res = (httplib::Response*)getPtr("handle");
		return res->has_header(name.c_str());
	}

	var response::getHeaderValue(varList args) {
		auto name = args[0].getString();
		auto id = args[1].getUInt64();
		auto res = (httplib::Response*)getPtr("handle");
		return res->get_header_value(name.c_str(), id);
	}

	var response::getHeaderValueCount(varList args) {
		auto name = args[0].getString();
		auto res = (httplib::Response*)getPtr("handle");
		return res->get_header_value_count(name.c_str());
	}

	var response::setHeader(varList args) {
		auto name = args[0].getString();
		auto value = args[1].getString();
		auto res = (httplib::Response*)getPtr("handle");
		res->set_header(name.c_str(), value);
		return var();
	}

	var response::setRedirect(varList args) {
		auto url = args[0].getString();
		auto res = (httplib::Response*)getPtr("handle");
		res->set_redirect(url.c_str());
		return var();
	}

	var response::setContent(varList args) {
		auto content = args[0].getString();
		auto type = args[1].getString();
		if (type.size() == 0) {
			// TODO: Maybe check content more?
			if (content[0] == '[' || content[0] == '{')
				type = "application/json";
			else
				type = "text/plain";
		}
		auto res = (httplib::Response*)getPtr("handle");
		res->set_content(content, type.c_str());
		return var();
	}

	var response::setContentProvider(varList args) {
		auto size = args[0].getUInt64();
		auto provider = ContentProvider((void (*)(
			size_t offset, size_t length, DataSink & sink)) args[1]
																			.getPtr());
		auto releaser =
			function<void()>((void (*)())args[2].getPtr());
		auto res = (httplib::Response*)getPtr("handle");
		res->set_content_provider(size, provider, releaser);
		return var();
	}

	var response::setChunkedContentProvider(varList args) {
		auto provider = function<void(size_t, DataSink&)>(
			(void (*)(size_t, DataSink&))args[0].getPtr());
		auto releaser =
			function<void()>((void (*)())args[1].getPtr());
		auto res = (httplib::Response*)getPtr("handle");
		res->set_chunked_content_provider(provider, releaser);
		return var();
	}

	request::request(const httplib::Request& r) : object(&proto) {
		setPtr("handle", (void*)&r);
		setString("path", r.path);
	}

	var request::hasHeader(varList args) {
		auto name = args[0].getString();
		auto req = (httplib::Request*)getPtr("handle");
		return req->has_header(name.c_str());
	}

	var request::getHeaderValue(varList args) {
		auto name = args[0].getString();
		auto id = args.size() >= 2 ? args[1].getUInt64() : 0;
		auto req = (httplib::Request*)getPtr("handle");
		return req->get_header_value(name.c_str(), id);
	}

	var request::getHeaderValueCount(varList args) {
		auto name = args[0].getString();
		auto req = (httplib::Request*)getPtr("handle");
		return req->get_header_value_count(name.c_str());
	}

	var request::hasParam(varList args) {
		auto name = args[0].getString();
		auto req = (httplib::Request*)getPtr("handle");
		return req->has_param(name.c_str());
	}

	var request::getParamValue(varList args) {
		auto name = args[0].getString();
		auto id = args.size() >= 2 ? args[1].getUInt64() : 0;
		auto req = (httplib::Request*)getPtr("handle");
		return req->get_param_value(name.c_str(), id);
	}

	var request::getParamValueCount(varList args) {
		auto name = args[0].getString();
		auto req = (httplib::Request*)getPtr("handle");
		return req->get_param_value_count(name.c_str());
	}

	var request::isMultipartFormData(varList args) {
		auto req = (httplib::Request*)getPtr("handle");
		return req->is_multipart_form_data();
	}

	var request::hasFile(varList args) {
		auto name = args[0].getString();
		auto req = (httplib::Request*)getPtr("handle");
		return req->has_file(name.c_str());
	}

	var request::getFileValue(varList args) {
		auto name = args[0].getString();
		auto req = (httplib::Request*)getPtr("handle");
		auto formData = req->get_file_value(name.c_str());
		return object({{"name", formData.name},
									 {"content", formData.content},
									 {"filename", formData.filename},
									 {"contentType", formData.content_type}});
	}

	var request::getMatches(varList args) {
		auto req = (httplib::Request*)getPtr("handle");
		auto arr = array();
		for (auto it = req->matches.begin();
				 it != req->matches.end();
				 ++it) {
			arr.pushString(it->str());
		}
		return arr;
	}
}  // namespace gold