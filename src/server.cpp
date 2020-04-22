#include "server.hpp"

#include <App.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>

#include "array.hpp"

namespace gold {
	using namespace std;
	using namespace uWS;
	namespace fs = std::filesystem;

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
		{"setErrorHandler", method(&server::setErrorHandler)},
		{"initialize", method(&server::initialize)},
		{"destroy", method(&server::destroy)},
	});

	object response::proto = object({
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

	object request::proto = object({
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

	var server::start(varList) {
		auto host = getString("host");
		auto port = getInt32("port");
		auto handle = (App*)getPtr("handle");
		if (handle != nullptr) {
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

	var server::get(varList args) {
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

	var server::post(varList args) {
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

	var server::put(varList args) {
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

	var server::patch(varList args) {
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

	var server::del(varList args) {
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

	var server::options(varList args) {
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

	var server::setMountPoint(varList args) {
		auto handle = (App*)getPtr("handle");
		auto mounts = getObject("mounts");
		auto point = fs::path(args[0].getString());
		if (!handle) return genericError("server handle null");

		point = fs::canonical(point);
		for (auto& p : fs::recursive_directory_iterator(point)) {
			std::string url = fs::relative(p.path().string());
			if (p.path().extension() != "") {
				if (url.find("index.html") != string::npos)
					url =
						url.substr(0, url.size() - strlen("index.html"));
				auto key = string(url.data(), url.length());
				auto buffer = string();
				ifstream fin(p.path().string(), std::ios::binary);
				fin.seekg(0, fin.end);
				auto fileSize = (size_t)fin.tellg();
				buffer.resize(fileSize);
				fin.seekg(0, fin.beg);
				fin.read(buffer.data(), buffer.length());
				mounts->setString(key, buffer);

				handle->get(
					"/" + url,
					[=](HttpResponse<false>* res, HttpRequest* req) {
						try {
							auto url = req->getUrl();
							auto file = mounts->getVar(string(url.substr(1)));
							if (file.getType() == typeString) {
								auto data = file.getString();
								res->writeStatus(HTTP_200_OK);
								auto ext = fs::path(url).extension().string();
								if (ext.find(".svg") != string::npos) {
									res->writeHeader(
										"Content-Type", "image/svg+xml");
								} else if (ext.find(".js") != string::npos) {
									res->writeHeader(
										"Content-Type", "text/javascript");
								} else if (ext.find(".css") != string::npos) {
									res->writeHeader("Content-Type", "text/css");
								}
								res->end({file});
							} else {
								res->writeStatus("404 Not Found");
								res->end();
							}
						} catch (genericError& e) {
							cerr << e << endl;
						}
					});
			}
		}

		return var();
	}

	var server::setErrorHandler(varList args) {
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

	var server::initialize(varList) {
		setPtr("handle", new App());
		setObject("mounts", object());
		return var();
	}

	var server::destroy(varList) {
		auto handle = (App*)getPtr("handle");
		if (handle) delete handle;
		return var();
	}

	server::server() : object(&proto) { initialize({}); }

	server::server(object config) : object(config, &proto) {
		initialize({});
	}

	response::response(HttpResponse<true>& res) : object(&proto) {
		setPtr("handle", &res);
		setBool("ssl", true);
	}

	response::response(HttpResponse<false>& res)
		: object(&proto) {
		setPtr("handle", &res);
		setBool("ssl", false);
	}

	var response::writeContinue(varList) {
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

	var response::writeStatus(varList args) {
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

	var response::writeHeader(varList args) {
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

	var response::end(varList args) {
		auto data = args[0].getString();
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			res->end(data);
		} else {
			auto res = (HttpResponse<false>*)getPtr("handle");
			res->end(data);
		}
		return var();
	}

	var response::tryEnd(varList args) {
		auto data = args[0].getString();
		auto size = args.size() >= 2 ? args[1].getInt32() : 0;
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			auto p = res->tryEnd(data, size);
			return var(gold::array({p.first, p.second}));
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		auto p = res->tryEnd(data, size);
		return var(gold::array({p.first, p.second}));
	}

	var response::write(varList args) {
		auto data = args[0].getString();
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			return res->write(data);
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		return res->write(data);
	}

	var response::getWriteOffset(varList) {
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			return res->getWriteOffset();
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		return res->getWriteOffset();
	}

	var response::hasResponded(varList) {
		auto ssl = getBool("ssl");
		if (ssl) {
			auto res = (HttpResponse<true>*)getPtr("handle");
			return res->hasResponded();
		}
		auto res = (HttpResponse<false>*)getPtr("handle");
		return res->hasResponded();
	}

	var response::cork(varList args) {
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

	var response::onWritable(varList args) {
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

	var response::onAborted(varList args) {
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

	var response::onData(varList args) {
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

	request::request(uWS::HttpRequest& req) : object(&proto) {
		setPtr("handle", (void*)&req);
		setString("path", string(req.getUrl()));
	}

	var request::getAllHeaders(varList) {
		auto req = (HttpRequest*)getPtr("handle");
		auto obj = object();
		auto it = req->begin();
		for (; it != req->end(); ++it) {
			auto k = string(it.ptr->key);
			auto v = string(it.ptr->value);
			auto exist = obj.getVar(k);
			if (exist.isString())
				obj.setArray(k, array({exist.getString(), v}));
			else if (exist.isArray())
				exist.getArray()->pushString(v);
			else
				obj.setString(k, v);
		}
		return obj;
	}

	var request::getHeader(varList args) {
		auto req = (HttpRequest*)getPtr("handle");
		auto lch = args[0].getString();
		return string(req->getHeader(lch));
	}

	var request::getMethod(varList) {
		auto req = (HttpRequest*)getPtr("handle");
		return string(req->getMethod());
	}

	var request::getParameter(varList args) {
		auto req = (HttpRequest*)getPtr("handle");
		auto in = args[0].getUInt32();
		return string(req->getParameter(in));
	}

	var request::getQuery(varList) {
		auto req = (HttpRequest*)getPtr("handle");
		return string(req->getQuery());
	}

	var request::getUrl(varList) {
		auto req = (HttpRequest*)getPtr("handle");
		return string(req->getUrl());
	}

	var request::getYield(varList) {
		auto req = (HttpRequest*)getPtr("handle");
		return req->getYield();
	}

	var request::setParameters(varList args) {
		auto req = (HttpRequest*)getPtr("handle");
		int in = args[0].getInt32();
		auto v = args[1].getString();
		auto pam = string_view(v);
		auto p = pair<int, std::string_view*>(in, &pam);
		req->setParameters(p);
		return var();
	}

	var request::setYield(varList args) {
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