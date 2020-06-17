#include "server.hpp"

#include <App.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>

#include "file.hpp"
#include "html.hpp"

namespace gold {
	using namespace std;
	using namespace uWS;
	namespace fs = std::filesystem;

	static auto mimeMap = map<string, string>({
		{".bin", "application/octet-stream"},
		{".zip", "application/zip"},
		{".rar", "application/x-rar-compressed"},
		{".json", "application/json"},
		{".bson", "application/bson"},
		{".js", "application/javascript"},
		{".xml", "application/xml"},
		{".gz", "application/gzip"},
		{".bz", "application/x-bzip"},
		{".bz2", "application/x-bzip2"},
		{".azw", "application/vnd.amazon.ebook"},
		{".doc", "application/msword"},
		{".ogx", "application/ogg"},
		{".pdf", "application/pdf"},
		{".tar", "application/x-tar"},
		{".xhtml", "application/xhtml+xml"},
		{".xls", "application/vnd.ms-excel"},
		{".7z", "application/x-7z-compressed"},
		{".abw", "application/x-abiword"},
		{".arc", "application/x-freearc"},
		{".html", "text/html"},
		{".csv", "text/csv"},
		{".css", "text/css"},
		{".rtf", "text/rtf"},
		{".txt", "text/plain"},
		{".ics", "text/calendar"},
		{".apng", "image/apng"},
		{".bmp", "image/bmp"},
		{".gif", "image/gif"},
		{".ico", "image/x-icon"},
		{".jpeg", "image/jpeg"},
		{".jpg", "image/jpeg"},
		{".jfif", "image/jpeg"},
		{".pjpeg", "image/jpeg"},
		{".pjp", "image/jpeg"},
		{".png", "image/png"},
		{".svg", "image/svg+xml"},
		{".tif", "image/tiff"},
		{".tiff", "image/tiff"},
		{".webp", "image/webp"},
		{".wav", "audio/wave"},
		{".aac", "audio/aac"},
		{".mp3", "audio/mpeg"},
		{".oga", "audio/ogg"},
		{".opus", "audio/opus"},
		{".weba", "audio/webm"},
		{".mid", "audio/midi"},
		{".midi", "audio/x-midi"},
		{".webm", "video/webm"},
		{".ogv", "video/ogg"},
		{".mpeg", "video/mpeg"},
		{".avi", "video/x-msvideo"},
		{".ttf", "font/ttf"},
		{".otf", "font/otf"},
		{".woff2", "font/woff2"},
		{".docx",
		 "application/"
		 "vnd.openxmlformats-officedocument.wordprocessingml."
		 "document"},
		{".ppt", "application/vnd.ms-powerpoint"},
		{".pptx",
		 "application/"
		 "vnd.openxmlformats-officedocument.presentationml."
		 "presentation"},
		{".xlsx",
		 "application/"
		 "vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
	});

	obj& server::getPrototype() {
		static auto proto = obj({
			{"host", "127.0.0.1"},
			{"port", 8080},
			{"cacheControl", "max-age=120"},
			{"start", method(&server::start)},
			{"get", method(&server::get)},
			{"post", method(&server::post)},
			{"put", method(&server::put)},
			{"patch", method(&server::patch)},
			{"del", method(&server::del)},
			{"options", method(&server::options)},
			{"setMountPoint", method(&server::setMountPoint)},
			{"setErrorHandler", method(&server::setErrorHandler)},
			{"initialize", method(&server::initialize)},
			{"destroy", method(&server::destroy)},
		});
		return proto;
	}

	obj& response::getPrototype() {
		static auto proto = obj({
			{"writeContinue", method(&response::writeContinue)},
			{"writeStatus", method(&response::writeStatus)},
			{"writeHeader", method(&response::writeHeader)},
			{"end", method(&response::end)},
			{"tryEnd", method(&response::tryEnd)},
			{"write", method(&response::write)},
			{"getWriteOffset", method(&response::getWriteOffset)},
			{"hasResponded", method(&response::hasResponded)},
			{"cork", method(&response::cork)},
			{"onWritable", method(&response::onWritable)},
			{"onAborted", method(&response::onAborted)},
			{"onData", method(&response::onData)},
		});
		return proto;
	}

	obj& request::getPrototype() {
		static auto proto = obj({
			{"getAllHeaders", method(&request::getAllHeaders)},
			{"getHeader", method(&request::getHeader)},
			{"getMethod", method(&request::getMethod)},
			{"getParameter", method(&request::getParameter)},
			{"getQuery", method(&request::getQuery)},
			{"getUrl", method(&request::getUrl)},
		});
		return proto;
	}

	var server::start(list) {
		auto host = getString("host");
		auto port = getInt32("port");
		auto handle = (App*)getPtr("handle");
		auto def = obj({});
		auto mounts = getObject("mounts");
		if (handle != nullptr) {
			auto handler = [&](auto* res, auto* req) {
				try {
					auto p = string(req->getUrl());
					auto f = mounts.getObject<file>(p);
					auto chash = req->getHeader("if-none-match");
					auto control = getString("cacheControl");
					if (f) {
						auto loaded = f.load();
						if (loaded.isView()) {
							auto hash = f.hash().getString();
							if (hash.compare(chash) == 0) {
								res->writeStatus("304 Not Changed");
								res->writeHeader("Cache-Control", control);
								res->end();
							} else {
								auto bin = loaded.getBinary();
								auto strView =
									string_view((char*)bin.data(), bin.size());
								res->writeStatus(HTTP_200_OK);
								auto ext = fs::path(p).extension().string();
								auto ct = mimeMap[ext];
								res->writeHeader("Content-Type", ct);
								res->writeHeader("Cache-Control", control);
								res->writeHeader("ETag", hash);
								res->end(strView);
							}
						} else {
							res->writeStatus("404 Not Found");
							res->end();
						}
					} else {
						res->writeStatus("404 Not Found");
						res->end();
					}
				} catch (genericError& e) {
					cerr << e << endl;
				}
			};

			for (auto it = mounts.begin(); it != mounts.end(); ++it) {
				auto url = it->first;
				handle->get(url, handler);
			}

			handle
				->listen(
					port,
					[port, host](auto* token) {
						if (token) {
							std::cout << "Serving " << host << " over "
												<< port << std::endl;
						}
					})
				.run();
			return var(true);
		}
		return var(false);
	}

	var server::get(list args) {
		auto handle = (App*)getPtr("handle");
		if (!handle) return genericError("server handle null");
		auto pattern = args[0].getString();
		auto func = args[1].getFunction();

		if (!func)
			return genericError("missing callback");
		else
			handle->get(pattern.c_str(), [=](auto* res, auto* req) {
				try {
					func({request(req), response(res)});
				} catch (genericError& e) {
					cerr << e << endl;
				}
			});
		return var();
	}

	var server::post(list args) {
		auto handle = (App*)getPtr("handle");
		if (!handle) return genericError("server handle null");
		auto pattern = args[0].getString();
		auto func = args[1].getFunction();
		if (!func)
			return genericError("missing callback");
		else
			handle->post(pattern.c_str(), [=](auto* res, auto* req) {
				try {
					func({request(req), response(res)});
				} catch (genericError& e) {
					cerr << e << endl;
				}
			});
		return var();
	}

	var server::put(list args) {
		auto handle = (App*)getPtr("handle");
		if (!handle) return genericError("server handle null");
		auto pattern = args[0].getString();
		auto func = args[1].getFunction();
		if (!func)
			return genericError("missing callback");
		else
			handle->put(pattern.c_str(), [=](auto* res, auto* req) {
				try {
					func({request(req), response(res)});
				} catch (genericError& e) {
					cerr << e << endl;
				}
			});
		return var();
	}

	var server::patch(list args) {
		auto handle = (App*)getPtr("handle");
		if (!handle) return genericError("server handle null");
		auto pattern = args[0].getString();
		auto func = args[1].getFunction();
		if (!func)
			return genericError("missing callback");
		else
			handle->patch(pattern.c_str(), [=](auto* res, auto* req) {
				try {
					func({request(req), response(res)});
				} catch (genericError& e) {
					cerr << e << endl;
				}
			});
		return var();
	}

	var server::del(list args) {
		auto handle = (App*)getPtr("handle");
		if (!handle) return genericError("server handle null");
		auto pattern = args[0].getString();
		auto func = args[1].getFunction();
		if (!func)
			return genericError("missing callback");
		else
			handle->del(pattern.c_str(), [=](auto* res, auto* req) {
				try {
					func({request(req), response(res)});
				} catch (genericError& e) {
					cerr << e << endl;
				}
			});
		return var();
	}

	var server::options(list args) {
		auto handle = (App*)getPtr("handle");
		if (!handle) return genericError("server handle null");
		auto pattern = args[0].getString();
		auto func = args[1].getFunction();
		if (!func)
			return genericError("missing callback");
		else
			handle->options(
				pattern.c_str(), [=](auto* res, auto* req) {
					try {
						func({request(req), response(res)});
					} catch (genericError& e) {
						cerr << e << endl;
					}
				});
		return var();
	}

	var server::setMountPoint(list args) {
		auto handle = (App*)getPtr("handle");
		if (!handle) return genericError("server handle null");
		auto mounts = getObject("mounts");
		for (auto it = args.begin(); it != args.end(); ++it) {
			auto point = fs::canonical(it->getString());
			file::recursiveReadDirectory(point, mounts);
		}

		return var();
	}

	var server::setErrorHandler(list args) {
		auto handle = (App*)getPtr("handle");
		if (!handle) return genericError("server handle null");
		auto func = args[0].getFunction();

		if (!func)
			return genericError("missing callback");
		else
			handle->get("/*", [=](auto* res, auto* req) {
				try {
					func({request(req), response(res)});
				} catch (genericError& e) {
					cerr << e << endl;
				}
			});
		return var();
	}

	var server::initialize(list) {
		auto kFN = getString("keyFileName");
		auto cFN = getString("certFileName");
		auto p = getString("passphrase");
		auto dh = getString("dhParamsFileName");
		auto ca = getString("caFileName");
		auto sslPreferLowMemoryUsage =
			getBool("sslPreferLowMemoryUsage", false);
		auto settings = us_socket_context_options_t{
			.key_file_name = (kFN.size() > 0) ? kFN.c_str() : nullptr,
			.cert_file_name =
				(cFN.size() > 0) ? cFN.c_str() : nullptr,
			.passphrase = (p.size() > 0) ? p.c_str() : nullptr,
			.dh_params_file_name =
				(dh.size() > 0) ? dh.c_str() : nullptr,
			.ca_file_name = (ca.size() > 0) ? ca.c_str() : nullptr,
			.ssl_prefer_low_memory_usage = sslPreferLowMemoryUsage,
		};
		setPtr("handle", new App(settings));
		auto mounts = obj({});
		setObject("mounts", mounts);
		return var();
	}

	var server::destroy(list) {
		auto handle = (App*)getPtr("handle");
		if (handle) delete handle;
		return var();
	}

	server::server() : obj() {}

	server::server(obj config) : obj() {
		copy(config);
		setParent(getPrototype());
		initialize({});
	}

	response::response() : obj() {}

	response::response(HttpResponse<true>* res) : obj() {
		setParent(getPrototype());
		setPtr("handle", res);
		setBool("ssl", true);
	}

	response::response(HttpResponse<false>* res) : obj() {
		setParent(getPrototype());
		setPtr("handle", res);
		setBool("ssl", false);
	}

	var response::writeContinue(list) {
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			res->writeContinue();
		} else {
			auto res = (HttpResponse<false>*)getPtr("handle");
			res->writeContinue();
		}
		return var();
	}

	var response::writeStatus(list args) {
		auto status = args[0].getString();
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			res->writeStatus(status);
		} else {
			auto res = (HttpResponse<false>*)getPtr("handle");
			res->writeStatus(status);
		}
		return var();
	}

	var response::writeHeader(list args) {
		auto key = args[0].getString();
		auto value = args[1];
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			if (value.isString()) {
				auto strValue = value.getString();
				res->writeHeader(key, strValue);
			} else if (value.isNumber()) {
				auto uValue = value.getUInt32();
				res->writeHeader(key, uValue);
			}
		} else {
			auto res = (HttpResponse<false>*)getPtr("handle");
			if (value.isString()) {
				auto strValue = value.getString();
				res->writeHeader(key, strValue);
			} else if (value.isNumber()) {
				auto uValue = value.getUInt32();
				res->writeHeader(key, uValue);
			}
		}
		return var();
	}

	var response::end(list args) {
		auto data = binary();
		if (args[0].isObject(HTML::iHTML::getPrototype())) {
			data = args[0].getObject<HTML::iHTML>();
			writeHeader({"Content-Type", "text/html"});
		} else if (args[0].isObject()) {
			data = args[0].getObject().getJSONBin();
			writeHeader({"Content-Type", "application/json"});
		} else if (args[0].isView())
			data = args[0].getBinary();

		auto strV = string_view((char*)data.data(), data.size());
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			res->end(strV);
		} else {
			auto res = (HttpResponse<false>*)getPtr("handle");
			res->end(strV);
		}
		return var();
	}

	var response::tryEnd(list args) {
		auto data = binary();
		if (args[0].isObject(HTML::iHTML::getPrototype())) {
			data = args[0].getObject<HTML::iHTML>();
			writeHeader({"Content-Type", "text/html"});
		} else if (args[0].isObject()) {
			data = args[0].getObject().getJSONBin();
			writeHeader({"Content-Type", "application/json"});
		} else if (args[0].isView())
			data = args[0].getBinary();

		auto strV = string_view((char*)data.data(), data.size());
		auto size = args.size() >= 2 ? args[1].getInt32() : 0;
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			auto p = res->tryEnd(strV, size);
			return var(gold::list({p.first, p.second}));
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		auto p = res->tryEnd(strV, size);
		return var(gold::list({p.first, p.second}));
	}

	var response::write(list args) {
		auto data = binary();
		if (args[0].isObject(HTML::iHTML::getPrototype())) {
			data = args[0].getObject<HTML::iHTML>();
			writeHeader({"Content-Type", "text/html"});
		} else if (args[0].isView())
			data = args[0].getBinary();
		auto strV = string_view((char*)data.data(), data.size());

		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			return res->write(strV);
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		return res->write(strV);
	}

	var response::getWriteOffset(list) {
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			return res->getWriteOffset();
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		return res->getWriteOffset();
	}

	var response::hasResponded(list) {
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			return res->hasResponded();
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		return res->hasResponded();
	}

	var response::cork(list args) {
		auto handler = args[0].getFunction();
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			res->cork([handler]() { handler({}); });
			return var();
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		res->cork([handler]() { handler({}); });
		return var();
	}

	var response::onWritable(list args) {
		auto handler = args[0].getFunction();
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			res->onWritable([=](int v) { return handler({v}); });
			return var();
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		res->onWritable([=](int v) { return handler({v}); });
		return var();
	}

	var response::onAborted(list args) {
		auto handler = args[0].getFunction();
		auto reqObj = args[1].getObject<request>();
		auto ssl = getBool("ssl");
		auto callback = [=, *this]() {
			handler({var(reqObj), var(*this)});
		};
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			res->onAborted(callback);
			return var();
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		res->onAborted(callback);
		return var();
	}

	var response::onData(list args) {
		struct dataContext {
			func handler;
			request req;
			response res;
			string buffer;
		};
		auto handler = args[0].getFunction();
		auto reqObj = args[1].getObject<request>();
		auto ssl = getBool("ssl");
		auto c = make_shared<dataContext>(
			dataContext{handler, reqObj, *this, string()});
		auto buffer = make_shared<string>();
		auto callback = [=](string_view v, bool b) {
			(c->buffer) = (c->buffer) + string(v);
			if (b) c->handler({c->buffer, c->req, c->res});
		};
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			res->onData(callback);
			return var();
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		res->onData(callback);
		return var();
	}

	request::request() : obj() {}

	request::request(uWS::HttpRequest* req) : obj() {
		setParent(getPrototype());
		auto headers = obj({});
		auto params = list({});
		auto it = req->begin();
		for (; it != req->end(); ++it) {
			if (it.ptr) {
				auto k = string(it.ptr->key);
				auto v = string(it.ptr->value);
				auto exist = headers.getVar(k);
				if (exist.isString()) {
					auto arr = list({exist.getString(), v});
					headers.setList(k, arr);
				} else if (exist.isList()) {
					auto arr = exist.getList();
					arr.pushString(v);
				} else
					headers.setString(k, v);
			}
		}
		size_t i = 0;
		auto currentParm = req->getParameter(i);
		while (currentParm.data() && currentParm.size() > 0) {
			params.pushString(string(currentParm));
			currentParm = req->getParameter(++i);
		}
		setObject("headers", headers);
		setList("params", params);
		setString("query", string(req->getQuery()));
		setString("method", string(req->getMethod()));
		setString("path", string(req->getUrl()));
		setPtr("handle", req);
	}

	var request::getAllHeaders() { return getObject("headers"); }

	var request::getHeader(list args) {
		auto headers = getObject("headers");
		auto lch = args[0].getString();
		transform(lch.begin(), lch.end(), lch.begin(), [](auto c) {
			return std::tolower(c);
		});
		return headers[lch];
	}

	var request::getMethod() { return getString("method"); }

	var request::getParameter(list args) {
		auto in = args[0].getUInt32();
		auto params = getList("params");
		return params[in];
	}

	var request::getQuery() { return getString("query"); }

	var request::getUrl() { return getString("path"); }

	var request::getYield() {
		auto handle = (uWS::HttpRequest*)getPtr("handle");
		return handle->getYield();
	}

	var request::setYield(list args) {
		auto handle = (uWS::HttpRequest*)getPtr("handle");
		auto value = args[0].getBool();
		handle->setYield(value);
		return value;
	}

	bool request::isWWWFormURLEncoded() {
		auto type = "application/x-www-form-urlencoded";
		auto contentType = getHeader({"Content-Type"}).getString();
		return contentType.find(type) != string::npos;
	}

	bool request::isJSON() {
		auto type0 = "application/json";
		auto type1 = "text/json";
		auto contentType = getHeader({"Content-Type"}).getString();
		return contentType.find(type0) != string::npos ||
					 contentType.find(type1) != string::npos;
	}

	bool request::acceptingJSON() {
		auto type0 = "application/json";
		auto type1 = "text/json";
		auto contentType = getHeader({"accept"}).getString();
		return contentType.find(type0) != string::npos ||
					 contentType.find(type1) != string::npos;
	}
	bool request::acceptingHTML() {
		auto type0 = "text/html";
		auto type1 = "application/xhtml+xml";
		auto contentType = getHeader({"accept"}).getString();
		return contentType.find(type0) != string::npos ||
					 contentType.find(type1) != string::npos;
	}

}  // namespace gold