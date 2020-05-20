#include "server.hpp"

#include <App.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>

#include "file.hpp"

namespace gold {
	using namespace std;
	using namespace uWS;
	namespace fs = std::filesystem;
	const auto CacheControl = "max-age=120";

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
			{"getYield", method(&request::getYield)},
			{"setParameters", method(&request::setParameters)},
			{"setYield", method(&request::setYield)},
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
			auto handler = [&](
											 HttpResponse<false>* res,
											 HttpRequest* req) {
				try {
					auto p = string(req->getUrl());
					auto f = mounts.getObject<file>(p);
					auto chash = req->getHeader("if-none-match");
					if (f) {
						auto loaded = f.load();
						if (loaded.isView()) {
							auto hash = f.hash().getString();
							if (hash.compare(chash) == 0) {
								res->writeStatus("304 Not Changed");
								res->writeHeader("Cache-Control", CacheControl);
								res->end();
							} else {
								auto bin = loaded.getBinary();
								auto strView =
									string_view((char*)bin.data(), bin.size());
								res->writeStatus(HTTP_200_OK);
								auto ext = fs::path(p).extension().string();
								auto ct = mimeMap[ext];
								res->writeHeader("Content-Type", ct);
								res->writeHeader("Cache-Control", CacheControl);
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
			handle->get(
				pattern.c_str(),
				[func](HttpResponse<false>* res, HttpRequest* req) {
					try {
						func({request(*req), response(*res)});
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
			handle->post(
				pattern.c_str(),
				[func](HttpResponse<false>* res, HttpRequest* req) {
					try {
						func({request(*req), response(*res)});
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
			handle->put(
				pattern.c_str(),
				[func](HttpResponse<false>* res, HttpRequest* req) {
					try {
						func({request(*req), response(*res)});
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
			handle->patch(
				pattern.c_str(),
				[func](HttpResponse<false>* res, HttpRequest* req) {
					try {
						func({request(*req), response(*res)});
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
			handle->del(
				pattern.c_str(),
				[func](HttpResponse<false>* res, HttpRequest* req) {
					try {
						func({request(*req), response(*res)});
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
				pattern.c_str(),
				[func](HttpResponse<false>* res, HttpRequest* req) {
					try {
						func({request(*req), response(*res)});
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
			handle->get(
				"/*",
				[func](HttpResponse<false>* res, HttpRequest* req) {
					try {
						func({request(*req), response(*res)});
					} catch (genericError& e) {
						cerr << e << endl;
					}
				});
		return var();
	}

	var server::initialize(list) {
		setPtr("handle", new App());
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

	response::response(HttpResponse<true>& res) : obj() {
		setParent(getPrototype());
		setPtr("handle", &res);
		setBool("ssl", true);
	}

	response::response(HttpResponse<false>& res) : obj() {
		setParent(getPrototype());
		setPtr("handle", &res);
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
		auto data = args[0].getBinary();
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
		auto data = args[0].getBinary();
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
		auto data = args[0].getString();
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			return res->write(data);
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		return res->write(data);
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
			res->onWritable(
				[handler](int v) { return handler({v}); });
			return var();
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		res->onWritable([handler](int v) { return handler({v}); });
		return var();
	}

	var response::onAborted(list args) {
		auto handler = args[0].getFunction();
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			res->onAborted([handler]() { handler({}); });
			return var();
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		res->onAborted([handler]() { handler({}); });
		return var();
	}

	var response::onData(list args) {
		auto handler = args[0].getFunction();
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			res->onData([handler](string_view v, bool b) {
				handler({string(v), var(b)});
			});
			return var();
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		res->onData([handler](string_view v, bool b) {
			handler({string(v), var(b)});
		});
		return var();
	}

	request::request() : obj() {}

	request::request(uWS::HttpRequest& req) : obj() {
		setParent(getPrototype());
		setPtr("handle", (void*)&req);
		setString("path", string(req.getUrl()));
	}

	var request::getAllHeaders(list) {
		auto req = (HttpRequest*)getPtr("handle");
		auto o = obj({});
		auto it = req->begin();
		for (; it != req->end(); ++it) {
			auto k = string(it.ptr->key);
			auto v = string(it.ptr->value);
			auto exist = o.getVar(k);
			if (exist.isString()) {
				auto arr = list({exist.getString(), v});
				o.setList(k, arr);
			} else if (exist.isList()) {
				auto arr = exist.getList();
				arr.pushString(v);
			} else
				o.setString(k, v);
		}
		return o;
	}

	var request::getHeader(list args) {
		auto req = (HttpRequest*)getPtr("handle");
		auto lch = args[0].getString();
		transform(lch.begin(), lch.end(), lch.begin(), [](auto c) {
			return std::tolower(c);
		});
		return string(req->getHeader(lch));
	}

	var request::getMethod(list) {
		auto req = (HttpRequest*)getPtr("handle");
		return string(req->getMethod());
	}

	var request::getParameter(list args) {
		auto req = (HttpRequest*)getPtr("handle");
		auto in = args[0].getUInt32();
		return string(req->getParameter(in));
	}

	var request::getQuery(list) {
		auto req = (HttpRequest*)getPtr("handle");
		return string(req->getQuery());
	}

	var request::getUrl(list) {
		auto req = (HttpRequest*)getPtr("handle");
		return string(req->getUrl());
	}

	var request::getYield(list) {
		auto req = (HttpRequest*)getPtr("handle");
		return req->getYield();
	}

	var request::setParameters(list args) {
		auto req = (HttpRequest*)getPtr("handle");
		int in = args[0].getInt32();
		auto v = args[1].getString();
		auto pam = string_view(v);
		auto p = pair<int, std::string_view*>(in, &pam);
		req->setParameters(p);
		return var();
	}

	var request::setYield(list args) {
		auto req = (HttpRequest*)getPtr("handle");
		auto y = args[0].getBool();
		req->setYield(y);
		return var();
	}

	bool request::isWWWFormURLEncoded() {
		auto wwwFormURLEncodedType =
			"application/x-www-form-urlencoded";
		auto contentType = getHeader({"Content-Type"}).getString();
		return contentType.find(wwwFormURLEncodedType) !=
					 string::npos;
	}

	bool request::isJSON() {
		auto wwwFormURLEncodedType = "application/json";
		auto contentType = getHeader({"Content-Type"}).getString();
		return contentType.find(wwwFormURLEncodedType) !=
					 string::npos;
	}

}  // namespace gold