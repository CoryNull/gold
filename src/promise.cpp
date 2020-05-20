#include "promise.hpp"

#include <unistd.h>

#include <iostream>
#include <mutex>
#include <thread>

namespace gold {

	static const uint64_t threadSleep = 60;
	static mutex promisesMutex;
	static mutex shouldQuitMutex;
	static mutex printMutex;
	static bool shouldQuit = false;
	static vector<promise> promises;
	static vector<thread> threads;

	object& promise::getPrototype() {
		static auto proto = obj({
			{"self", var()},
			{"method", var()},
			{"response", var()},
			{"status", 0},
			{"addArgs", method(&promise::addArgs)},
			{"call", method(&promise::call)},
			{"await", method(&promise::await)},
		});
		return proto;
	}

	bool shouldContinue() {
		unique_lock<mutex> gaurd(shouldQuitMutex);
		auto keepWorking = !shouldQuit;
		return keepWorking;
	}

	bool getPromise(promise& job) {
		unique_lock<mutex> gaurd(promisesMutex);
		auto it = promises.begin();
		if (it != promises.end()) {
			job = *it;
			promises.erase(it);
			return true;
		}
		return false;
	}

	void pushPromise(promise& job) {
		unique_lock<mutex> gaurd(promisesMutex);
		promises.push_back(job);
	}

	void printError(string msg) {
		unique_lock<mutex> gaurd(printMutex);
		cerr << msg << endl;
	}

	bool singleThread() {
		auto size = threads.size();
		return size == 0;
	}

	int promise::worker() {
		auto job = promise();
		try {
			while (shouldContinue()) {
				try {
					if (getPromise(job)) job.call();
					job = promise();
					usleep(threadSleep);
				} catch (exception e) {
					printError(e.what());
				}
			}
		} catch (exception e) {
			printError(e.what());
			return -1;
		}

		return 0;
	}

	void promise::useAllCores() {
		for (uint32_t i = 0; i < thread::hardware_concurrency();
				 ++i)
			threads.push_back(thread(worker));
	}

	void promise::joinThreads() {
		shouldQuitMutex.lock();
		shouldQuit = true;
		shouldQuitMutex.unlock();
		for (auto it = threads.begin(); it != threads.end(); ++it)
			it->join();
		threads.clear();
		shouldQuitMutex.lock();
		shouldQuit = false;
		shouldQuitMutex.unlock();
	}

	promise::promise() : object() {}

	promise::promise(const promise& copy) : object(copy) {
		setParent(getPrototype());
	}

	promise::promise(object self, method m, list args)
		: object() {
		setParent(getPrototype());
		setObject("self", self);
		setMethod("method", m);
		setList("args", args);
		if (!singleThread()) pushPromise(*this);
	}

	promise::promise(object self, func f, list args) : object() {
		setParent(getPrototype());
		setObject("self", self);
		setFunc("func", f);
		setList("args", args);
		if (!singleThread()) pushPromise(*this);
	}

	promise::~promise() {
		if (data && data.use_count() == 1) {
			empty();
		}
		data = nullptr;
	}

	var promise::addArgs(list args) {
		auto cArgs = list();
		assignList("args", cArgs);
		cArgs += args;
		return cArgs;
	}

	var promise::call(list) {
		auto obj = getObject("self");
		auto args = getList("args");
		auto resp = var();
		if (getType("method") == typeMethod) {
			auto m = getMethod("method");
			resp = (obj.*m)(args);
			erase("method");
			erase("self");
			erase("args");
		} else if (getType("func") == typeFunction) {
			auto f = getFunc("func");
			auto apArgs = list({obj});
			apArgs += args;  // obj, args...
			resp = (f)(apArgs);
			erase("func");
			erase("self");
			erase("args");
		} else {
			setInt8("status", -1);
		}
		setVar("response", resp);
		setInt8("status", 1);
		return resp;
	}

	var promise::await(list) {
		if (singleThread()) {
			return call();
		} else {
			auto status = getInt8("status");
			while (status == 0) {
				usleep(threadSleep);
				status = getInt8("status");
			}
			return getVar("response");
		}
	}

	promise::operator bool() {
		auto status = getInt8("status");
		return status != 0;
	}
}  // namespace gold