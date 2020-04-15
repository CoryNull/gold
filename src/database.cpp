#include "database.hpp"

#include <mongoc.h>

#include <array.hpp>
#include <cstdint>
#include <iostream>
#include <vector>

namespace gold {

	array arrayFromBSON(bson_t* b);
	object objectFromBSON(bson_t* b);

	bson_t* newBSONFromObject(object& obj) {
		auto bin = obj.getBSON();
		return bson_new_from_data(bin.data(), bin.size());
	}

	array arrayFromBSON(bson_t* b) {
		auto a = array();
		bson_iter_t iter;
		if (bson_iter_init(&iter, b)) {
			while (bson_iter_next(&iter)) {
				auto key = (uint64_t)stol(bson_iter_key(&iter));
				switch (bson_iter_type(&iter)) {
					case BSON_TYPE_DOUBLE:
						a.setDouble(key, bson_iter_as_double(&iter));
						break;
					case BSON_TYPE_UTF8: {
						uint32_t s = 0;
						auto data = bson_iter_utf8(&iter, &s);
						a.setString(key, string(data, s));
						break;
					}
					case BSON_TYPE_DOCUMENT: {
						const uint8_t* loc = nullptr;
						uint32_t s = 0;
						bson_iter_document(&iter, &s, &loc);
						auto bs = bson_new_from_data(loc, s);
						a.setObject(key, objectFromBSON(bs));
						bson_destroy(bs);
						break;
					}
					case BSON_TYPE_ARRAY: {
						const uint8_t* loc = nullptr;
						uint32_t s = 0;
						bson_iter_array(&iter, &s, &loc);
						auto bs = bson_new_from_data(loc, s);
						a.setArray(key, arrayFromBSON(bs));
						bson_destroy(bs);
						break;
					}
					case BSON_TYPE_BINARY: {
						bson_subtype_t st;
						uint32_t s = 0;
						const uint8_t* data = nullptr;
						bson_iter_binary(&iter, &st, &s, &data);
						a.setString(key, string((char*)data, s));
						break;
					}
					case BSON_TYPE_OID: {
						auto oid = bson_iter_oid(&iter);
						a.setString(key, string((char*)oid->bytes, 12));
						break;
					}
					case BSON_TYPE_BOOL:
						a.setBool(key, bson_iter_bool(&iter));
						break;
					case BSON_TYPE_DATE_TIME:
						a.setInt64(key, bson_iter_date_time(&iter));
						break;
					case BSON_TYPE_REGEX: {
						const char* opts = nullptr;
						auto reg = bson_iter_regex(&iter, &opts);
						a.setArray(key, array({reg, opts}));
						break;
					}
					case BSON_TYPE_CODE: {
						uint32_t size = 0;
						auto code = bson_iter_code(&iter, &size);
						a.setString(key, code);
						break;
					}
					case BSON_TYPE_CODEWSCOPE: {
						uint32_t cSize = 0;
						uint32_t sSize = 0;
						const uint8_t* scope;
						auto code = bson_iter_codewscope(
							&iter, &cSize, &sSize, &scope);
						a.setArray(
							key,
							array({string(code, cSize),
										 string((char*)scope, sSize)}));
						break;
					}
					case BSON_TYPE_INT32:
						a.setInt32(key, bson_iter_int32(&iter));
						break;
					case BSON_TYPE_TIMESTAMP: {
						union {
							struct {
								uint32_t stamp;
								uint32_t inc;
							} pair;
							uint64_t data;
						};

						bson_iter_timestamp(&iter, &pair.stamp, &pair.inc);
						a.setUInt64(key, data);
						break;
					}
					case BSON_TYPE_INT64:
						a.setInt64(key, bson_iter_int64(&iter));
						break;
					case BSON_TYPE_DECIMAL128: {
						bson_decimal128_t d;
						if (bson_iter_decimal128(&iter, &d)) {
							char str[BSON_DECIMAL128_STRING];
							bson_decimal128_to_string(&d, str);
							a.setString(
								key,
								string((char*)str, BSON_DECIMAL128_STRING));
						}
						break;
					}
					case BSON_TYPE_MAXKEY:
					case BSON_TYPE_MINKEY:
					case BSON_TYPE_SYMBOL:
					case BSON_TYPE_DBPOINTER:
					case BSON_TYPE_UNDEFINED:
					case BSON_TYPE_NULL:
					default:
						a.setNull(key);
						break;
				}
			}
		}
		return a;
	}

	object objectFromBSON(bson_t* b) {
		auto obj = object();
		bson_iter_t iter;
		if (bson_iter_init(&iter, b)) {
			while (bson_iter_next(&iter)) {
				auto key = bson_iter_key(&iter);
				switch (bson_iter_type(&iter)) {
					case BSON_TYPE_DOUBLE:
						obj.setDouble(key, bson_iter_as_double(&iter));
						break;
					case BSON_TYPE_UTF8: {
						uint32_t s = 0;
						auto data = bson_iter_utf8(&iter, &s);
						obj.setString(key, string(data, s));
						break;
					}
					case BSON_TYPE_DOCUMENT: {
						const uint8_t* loc = nullptr;
						uint32_t s = 0;
						bson_iter_document(&iter, &s, &loc);
						auto bs = bson_new_from_data(loc, s);
						obj.setObject(key, objectFromBSON(bs));
						bson_destroy(bs);
						break;
					}
					case BSON_TYPE_ARRAY: {
						const uint8_t* loc = nullptr;
						uint32_t s = 0;
						bson_iter_array(&iter, &s, &loc);
						auto bs = bson_new_from_data(loc, s);
						obj.setArray(key, arrayFromBSON(bs));
						bson_destroy(bs);
						break;
					}
					case BSON_TYPE_BINARY: {
						bson_subtype_t st;
						uint32_t s = 0;
						const uint8_t* data = nullptr;
						bson_iter_binary(&iter, &st, &s, &data);
						obj.setString(key, string((char*)data, s));
						break;
					}
					case BSON_TYPE_OID: {
						auto oid = bson_iter_oid(&iter);
						obj.setString(key, string((char*)oid->bytes, 12));
						break;
					}
					case BSON_TYPE_BOOL:
						obj.setBool(key, bson_iter_bool(&iter));
						break;
					case BSON_TYPE_DATE_TIME:
						obj.setInt64(key, bson_iter_date_time(&iter));
						break;
					case BSON_TYPE_REGEX: {
						const char* opts = nullptr;
						auto reg = bson_iter_regex(&iter, &opts);
						obj.setArray(key, array({reg, opts}));
						break;
					}
					case BSON_TYPE_CODE: {
						uint32_t size = 0;
						auto code = bson_iter_code(&iter, &size);
						obj.setString(key, code);
						break;
					}
					case BSON_TYPE_CODEWSCOPE: {
						uint32_t cSize = 0;
						uint32_t sSize = 0;
						const uint8_t* scope;
						auto code = bson_iter_codewscope(
							&iter, &cSize, &sSize, &scope);
						obj.setArray(
							key,
							array({string(code, cSize),
										 string((char*)scope, sSize)}));
						break;
					}
					case BSON_TYPE_INT32:
						obj.setInt32(key, bson_iter_int32(&iter));
						break;
					case BSON_TYPE_TIMESTAMP: {
						union {
							struct {
								uint32_t stamp;
								uint32_t inc;
							} pair;
							uint64_t data;
						};

						bson_iter_timestamp(&iter, &pair.stamp, &pair.inc);
						obj.setUInt64(key, data);
						break;
					}
					case BSON_TYPE_INT64:
						obj.setInt64(key, bson_iter_int64(&iter));
						break;
					case BSON_TYPE_DECIMAL128: {
						bson_decimal128_t d;
						if (bson_iter_decimal128(&iter, &d)) {
							char str[BSON_DECIMAL128_STRING];
							bson_decimal128_to_string(&d, str);
							obj.setString(key, string(str));
						}
						break;
					}
					case BSON_TYPE_MAXKEY:
					case BSON_TYPE_MINKEY:
					case BSON_TYPE_SYMBOL:
					case BSON_TYPE_DBPOINTER:
					case BSON_TYPE_UNDEFINED:
					case BSON_TYPE_NULL:
					default:
						obj.setNull(key);
						break;
				}
			}
		}
		return obj;
	}

	var varFromBSON(bson_t* b) {
		if (bson_has_field(b, "0")) {
			// Array?
			return var(arrayFromBSON(b));
		}
		return var(objectFromBSON(b));
	}

	object database::proto = object({
		{"host", "mongodb://localhost:27017"},
		{"appName", "gold-app"},
		{"name", "db_name"},
		{"connect", method(&database::connect)},
		{"disconnect", method(&database::disconnect)},
		{"destroy", method(&database::destroy)},
		{"createCollection", method(&database::createCollection)},
		{"getCollection", method(&database::getCollection)},
		{"getDatabaseNames", method(&database::getDatabaseNames)},
	});

	database::database() : object(&proto) {}

	database::database(object config) : object(config, &proto) {}

	var database::connect(varList args) {
		static bool inited = false;
		if (inited == false) {
			inited = true;
			mongoc_init();
		}
		bson_error_t error;
		auto uriString = getString("host");
		auto uri =
			mongoc_uri_new_with_error(uriString.c_str(), &error);
		if (!uri) {
			cerr << "failed to parse URI:	" << uriString << endl
					 << "error message:				" << error.message << endl;
			return genericError(error.message);
		}
		auto pool = mongoc_client_pool_new(uri);
		if (!pool) return genericError("failed to create pool");
		auto appName = getString("appName");
		mongoc_client_pool_set_appname(pool, appName.c_str());
		setPtr("pool", pool);
		auto namesReturn = getDatabaseNames();
		auto dbName = getString("name");
		if (namesReturn.isArray()) {
			auto dbNames = namesReturn.getArray();
			cout << "connection made: " << uriString << endl
					 << "databases: " << dbNames->getJSON() << endl;
		} else {
			auto exc = namesReturn.getError();
			cerr << *exc << endl;
		}
		auto client = mongoc_client_pool_pop(pool);
		auto db =
			mongoc_client_get_database(client, dbName.c_str());
		mongoc_client_pool_push(pool, client);
		setPtr("handle", db);
		mongoc_uri_destroy(uri);
		return var();
	}

	var database::disconnect(varList args) {
		auto pool = (mongoc_client_pool_t*)getPtr("pool");
		auto db = (mongoc_database_t*)getPtr("handle");
		mongoc_database_destroy(db);
		mongoc_client_pool_destroy(pool);
		return var();
	}

	var database::destroy(varList args) {
		disconnect();
		mongoc_cleanup();
		return var();
	}

	var database::getDatabaseNames(varList args) {
		auto pool = (mongoc_client_pool_t*)getPtr("pool");
		auto client = mongoc_client_pool_pop(pool);
		bson_t* opts = nullptr;
		if (args.size() > 1) {
			auto optsObj = args[0].getObject();
			opts = newBSONFromObject(*optsObj);
		}
		bson_error_t error;
		auto strV = mongoc_client_get_database_names_with_opts(
			client, opts, &error);
		mongoc_client_pool_push(pool, client);
		if (opts) bson_destroy(opts);
		if (strV) {
			auto strings = array();
			auto i = 0;
			auto str = strV[i];
			while (str != nullptr) {
				strings.pushString(str);
				str = strV[i++];
			}
			bson_strfreev(strV);
			return strings;
		}
		return genericError(error.message);
	}

	var database::createCollection(varList args) {
		auto db = (mongoc_database_t*)getPtr("handle");
		auto name = args[0].getString();
		auto optsObj = args[1].getObject();
		bson_t* opts =
			optsObj ? newBSONFromObject(*optsObj) : nullptr;
		bson_error_t error;
		auto col = (struct _mongoc_collection_t*)
			mongoc_database_create_collection(
				db, name.c_str(), opts, &error);
		if (col) return var(collection(this, col));
		return genericError(error.message);
	}

	var database::getCollection(varList args) {
		auto pool = (mongoc_client_pool_t*)getPtr("pool");
		auto client = mongoc_client_pool_pop(pool);
		auto name = args[0].getString();
		auto dbName = getString("name");
		auto col = (struct _mongoc_collection_t*)
			mongoc_client_get_collection(
				client, dbName.c_str(), name.c_str());
		mongoc_client_pool_push(pool, client);
		if (col) return var(collection(this, col));
		return var();
	}

	object collection::proto = object({
		{"name", "undefined"},
		{"addIndexes", method(&collection::addIndexes)},
		{"dropIndex", method(&collection::dropIndex)},
		{"deleteOne", method(&collection::deleteOne)},
		{"deleteMany", method(&collection::deleteMany)},
		{"findOne", method(&collection::findOne)},
		{"findMany", method(&collection::findMany)},
		{"updateOne", method(&collection::updateOne)},
		{"updateMany", method(&collection::updateMany)},
		{"insert", method(&collection::insert)},
		{"replace", method(&collection::replace)},
		{"rename", method(&collection::rename)},
		{"destroy", method(&collection::destroy)},
	});

	collection::collection() : object(&proto) {}

	collection::collection(
		database* d, struct _mongoc_collection_t* c)
		: object(&proto) {
		setPtr("handle", c);
		setObject("database", *d);
		setString(
			"name",
			mongoc_collection_get_name((mongoc_collection_t*)c));
	}

	var collection::addIndexes(varList args) {
		auto handle = (mongoc_collection_t*)getPtr("handle");
		auto cName = args[0].getString();
		auto keys = args[1].getObject();
		auto optsObj = args[2].getObject();
		if (cName.size() == 0)
			return genericError(
				"Missing collection name for first arg");
		if (!keys)
			return genericError("Missing keys from second arg");
		bson_t* opts =
			optsObj ? newBSONFromObject(*optsObj) : nullptr;
		auto bKeys = newBSONFromObject(*keys);
		auto indexName =
			mongoc_collection_keys_to_index_string(bKeys);
		auto command = object({
			{"createIndexes", cName},
			{
				"indexes",
				var(array({object({
					{"key", (*keys)},
					{"name", (indexName)},
				})})),
			},
		});
		bson_error_t error;
		auto cBson = newBSONFromObject(command);
		bson_t replyBson;
		auto success = mongoc_collection_write_command_with_opts(
			handle, cBson, opts, &replyBson, &error);
		bson_destroy(cBson);
		if (opts) bson_destroy(opts);
		if (!success) {
			return genericError(error.message);
		}
		auto reply = varFromBSON(&replyBson);
		return reply;
	}

	var collection::dropIndex(varList args) {
		auto handle = (mongoc_collection_t*)getPtr("handle");
		auto indexName = args[0].getString();
		bson_error_t err;
		if (mongoc_collection_drop_index(
					handle, indexName.c_str(), &err))
			return true;
		return var(genericError(err.message));
	}

	var collection::deleteOne(varList args) {
		auto handle = (mongoc_collection_t*)getPtr("handle");
		auto selObj = args[0].getObject();
		auto optObj = args[1].getObject();
		if (!selObj)
			return genericError("Missing selector for first arg");
		bson_t* opts =
			optObj ? newBSONFromObject(*optObj) : nullptr;
		auto sel = newBSONFromObject(*selObj);
		bson_t replyBson;
		bson_error_t err;
		auto success = mongoc_collection_delete_one(
			handle, sel, opts, &replyBson, &err);
		bson_destroy(sel);
		if (opts) bson_destroy(opts);
		if (!success) {
			return genericError(err.message);
		}
		auto reply = varFromBSON(&replyBson);
		return reply;
	}

	var collection::deleteMany(varList args) {
		auto handle = (mongoc_collection_t*)getPtr("handle");
		auto selObj = args[0].getObject();
		auto optObj = args[1].getObject();
		if (!selObj)
			return genericError("Missing selector for first arg");
		bson_t* opts =
			optObj ? newBSONFromObject(*optObj) : nullptr;
		auto sel = newBSONFromObject(*selObj);
		bson_t replyBson;
		bson_error_t err;
		auto success = mongoc_collection_delete_many(
			handle, sel, opts, &replyBson, &err);
		bson_destroy(sel);
		if (opts) bson_destroy(opts);
		if (!success) return genericError(err.message);
		auto reply = varFromBSON(&replyBson);
		return reply;
	}

	var collection::findOne(varList args) {
		static auto def = object(object_list{{"limit", 1}});
		auto handle = (mongoc_collection_t*)getPtr("handle");
		auto selObj = args[0].getObject();
		auto optObj =
			args.size() >= 2 ? args[1].getObject() : nullptr;
		if (optObj)
			optObj->setInt32("limit", 1);
		else
			optObj = &def;
		if (!selObj)
			return genericError("Missing selector for first arg");
		bson_t* opts = newBSONFromObject(*optObj);
		auto sel = newBSONFromObject(*selObj);
		bson_t replyBson;
		bson_error_t err;
		auto cursor = mongoc_collection_find_with_opts(
			handle, sel, opts, nullptr);
		bson_destroy(sel);
		bson_destroy(opts);
		const bson_t* doc = nullptr;
		var resp;
		while (mongoc_cursor_next(cursor, &doc)) {
			auto resi = varFromBSON((bson_t*)doc);
			if (resi.isObject()) resp = *resi.getObject();
		}
		if (mongoc_cursor_error(cursor, &err)) {
			mongoc_cursor_destroy(cursor);
			return genericError(err.message);
		}
		mongoc_cursor_destroy(cursor);
		return resp;
	}

	var collection::findMany(varList args) {
		auto handle = (mongoc_collection_t*)getPtr("handle");
		auto selObj = args[0].getObject();
		auto optObj = args[1].getObject();
		if (!selObj)
			return genericError("Missing selector for first arg");
		bson_t* opts =
			optObj ? newBSONFromObject(*optObj) : nullptr;
		auto sel = newBSONFromObject(*selObj);
		bson_t replyBson;
		bson_error_t err;
		auto cursor = mongoc_collection_find_with_opts(
			handle, sel, opts, nullptr);
		bson_destroy(sel);
		if (opts) bson_destroy(opts);
		const bson_t* doc = nullptr;
		auto response = array();
		while (mongoc_cursor_next(cursor, &doc)) {
			auto resi = varFromBSON((bson_t*)doc);
			if (resi.isArray())
				response.pushArray(*resi.getArray());
			else if (resi.isObject())
				response.pushObject(*resi.getObject());
		}
		if (mongoc_cursor_error(cursor, &err)) {
			mongoc_cursor_destroy(cursor);
			auto msg = string(err.message);
			return genericError(msg);
		}
		mongoc_cursor_destroy(cursor);
		return response;
	}

	var collection::updateOne(varList args) {
		auto handle = (mongoc_collection_t*)getPtr("handle");
		auto selObj = args[0].getObject();
		auto upObj = args[1].getObject();
		auto optObj = args[2].getObject();
		if (!selObj)
			return genericError("Missing selector for first arg");
		if (!upObj)
			return genericError(
				"Missing update object for second arg");
		bson_t* opts =
			optObj ? newBSONFromObject(*optObj) : nullptr;
		auto sel = newBSONFromObject(*selObj);
		auto up = newBSONFromObject(*upObj);
		bson_t replyBson;
		bson_error_t err;
		auto success = mongoc_collection_update_one(
			handle, sel, up, opts, &replyBson, &err);
		bson_destroy(sel);
		bson_destroy(up);
		if (opts) bson_destroy(opts);
		if (!success) {
			return genericError(err.message);
		}
		auto reply = varFromBSON(&replyBson);
		return reply;
	}

	var collection::updateMany(varList args) {
		auto handle = (mongoc_collection_t*)getPtr("handle");
		auto selObj = args[0].getObject();
		auto upObj = args[1].getObject();
		auto optObj = args[2].getObject();
		if (!selObj)
			return genericError("Missing selector for first arg");
		if (!upObj)
			return genericError(
				"Missing update object for second arg");
		bson_t* opts =
			optObj ? newBSONFromObject(*optObj) : nullptr;
		auto sel = newBSONFromObject(*selObj);
		auto up = newBSONFromObject(*upObj);
		bson_t replyBson;
		bson_error_t err;
		auto success = mongoc_collection_update_many(
			handle, sel, up, opts, &replyBson, &err);
		bson_destroy(sel);
		bson_destroy(up);
		if (opts) bson_destroy(opts);
		if (!success) {
			return genericError(err.message);
		}
		auto reply = varFromBSON(&replyBson);
		return reply;
	}

	var collection::insert(varList args) {
		auto handle = (mongoc_collection_t*)getPtr("handle");
		auto objData = args[0].getObject();
		auto optObj = args[1].getObject();
		if (!objData)
			return genericError("Missing object for first arg");
		auto obj = newBSONFromObject(*objData);
		bson_t* opts =
			optObj ? newBSONFromObject(*optObj) : nullptr;
		bson_t replyBson;
		bson_error_t err;
		auto success = mongoc_collection_insert_one(
			handle, obj, opts, &replyBson, &err);
		bson_destroy(obj);
		if (opts) bson_destroy(opts);
		if (!success) return genericError(err.message);
		auto reply = varFromBSON(&replyBson);
		return reply;
	}

	var collection::replace(varList args) {
		auto handle = (mongoc_collection_t*)getPtr("handle");
		auto selObj = args[0].getObject();
		auto upObj = args[1].getObject();
		auto optObj = args[2].getObject();
		if (!selObj)
			return genericError("Missing selector for first arg");
		if (!upObj)
			return genericError(
				"Missing update object for second arg");
		bson_t* opts =
			optObj ? newBSONFromObject(*optObj) : nullptr;
		auto sel = newBSONFromObject(*selObj);
		auto up = newBSONFromObject(*upObj);
		bson_t replyBson;
		bson_error_t err;
		auto success = mongoc_collection_replace_one(
			handle, sel, up, opts, &replyBson, &err);
		bson_destroy(sel);
		bson_destroy(up);
		if (opts) bson_destroy(opts);
		if (!success) {
			return genericError(err.message);
		}
		auto reply = varFromBSON(&replyBson);
		return reply;
	}

	var collection::rename(varList args) {
		auto handle = (mongoc_collection_t*)getPtr("handle");
		auto db = getDatabase();
		auto dbName = db->getString("name");
		auto newName = args[0].getString();
		bson_error_t err;
		auto success = mongoc_collection_rename(
			handle, dbName.c_str(), newName.c_str(), false, &err);
		if (!success) {
			return genericError(err.message);
		}
		setString("name", newName);
		return newName;
	}

	var collection::destroy(varList args) {
		auto handle = (mongoc_collection_t*)getPtr("handle");
		mongoc_collection_destroy(handle);
		return var();
	}

	database* collection::getDatabase() {
		return (database*)getObject("database");
	}

	object model::proto = object({
		{"_id", ""},
		{"updated", 0},
		{"created", 0},
		{"save", method(&model::save)},
		{"remove", method(&model::remove)},
	});

	model::model(collection* c, object* parent) : object(parent) {
		if (parent) parent->setObject("col", *c);
	}

	model::model(collection* c, object data, object* parent)
		: object(data, parent) {
		if (parent) parent->setObject("col", *c);
	}

	var model::save(varList args) {
		auto col = getCollection();
		auto id = getString("_id");
		setInt64("updated", getMonoTime());
		if (id.empty()) {
			auto res = col->insert({*this});
			if (res.isError())
				return res;
			else if (res.isObject())
				this->copy(*res.getObject());
			return *this;
		}
		setInt64("created", getMonoTime());
		auto res = col->updateOne({object{{"_id", id}}, *this});
		if (res.isError())
			return res;
		else if (res.isObject())
			this->copy(*res.getObject());
		return *this;
	}

	var model::remove(varList args) {
		auto col = getCollection();
		return col->deleteOne({object{{"_id", getString("_id")}}});
	}

	database* model::getDatabase() {
		return (database*)getCollection()->getObject("database");
	}

	collection* model::getCollection() {
		return (collection*)getObject("col");
	}

	string model::getID() { return getString("_id"); }

	int64_t getMonoTime() { return bson_get_monotonic_time(); }

}  // namespace gold