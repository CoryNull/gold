#pragma once

#include "object.hpp"

namespace gold {
	class database : public object {
	 protected:
		static object proto;

	 public:
		database();
		database(object config);

		var connect(varList args = {});
		var disconnect(varList args = {});
		var destroy(varList args = {});

		var getDatabaseNames(varList args = {});

		var createCollection(varList args);
		var getCollection(varList args);
	};

	class collection : public object {
	 protected:
		friend class model;
		static object proto;

	 public:
	 	collection();
		collection(database* db, struct _mongoc_collection_t*);

		var addIndexes(varList args);
		var dropIndex(varList args);
		var deleteOne(varList args);
		var deleteMany(varList args);
		var findOne(varList args);
		var findMany(varList args);
		var updateOne(varList args);
		var updateMany(varList args);
		var insert(varList args);
		var replace(varList args);
		var rename(varList args);
		var destroy(varList args);

		database* getDatabase();
	};

	class model : public object {
	 public:
		static object proto;
		model(collection* col, object* parent = nullptr);
		model(
			collection* col, object data, object* parent = nullptr);

		var save(varList args = {});
		var remove(varList args = {});

		database* getDatabase();
		collection* getCollection();
		string getID();
	};

	int64_t getMonoTime();
}  // namespace gold