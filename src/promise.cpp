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
		shouldQuitMutex.lock();
		auto keepWorking = !shouldQuit;
		shouldQuitMutex.unlock();
		return keepWorking;
	}

	bool getPromise(promise& job) {
		promisesMutex.lock();
		auto back = promises.rbegin();
		if (back != promises.rend()) {
			job = *back;
			promises.pop_back();
			promisesMutex.unlock();
			return true;
		}
		promisesMutex.unlock();
		return false;
	}

	void pushPromise(promise& job) {
		promisesMutex.lock();
		promises.push_back(job);
		promisesMutex.unlock();
	}

	void printError(string msg) {
		printMutex.lock();
		cerr << msg << endl;
		printMutex.unlock();
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
		returnList("args", cArgs);
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
			setInt8("status", 1);
		} else if (getType("func") == typeFunction) {
			auto f = getFunc("func");
			auto apArgs = list({obj});
			apArgs += args;  // obj, args...
			resp = (f)(apArgs);
			setInt8("status", 1);
		} else {
			setInt8("status", -1);
		}
		setVar("response", resp);
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