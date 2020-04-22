#include "worker.hpp"

#include <iostream>

namespace gold {
	class worker::job {
	 public:
		method m;
		object& o;
		varList a;
		job(method me, object& ob, varList ar)
			: m(me), o(ob), a(ar) {}
		job(const job& copy) : m(copy.m), o(copy.o), a(copy.a) {}
		~job() {}
		void call() {
			auto ret = (o.*m)(varList());
			if (ret.isError()) cerr << ret << endl;
		}
	};

	int worker::workerProcess(worker* w) {
		do {
			auto job = w->nextJob();
			if (job) job->call();
		} while (!w->shouldDie());
		return 0;
	}

	worker::jobPtr worker::nextJob() {
		if (mtx.try_lock()) {
			auto it = jobs.begin();
			if (it != jobs.end()) {
				auto j = it.operator*();
				jobs.erase(it);
				mtx.unlock();
				if (j) return j;
			}
			mtx.unlock();
		}
		return jobPtr();
	}

	bool worker::shouldDie() { return this->kill == true; }

	worker::worker() : jobs(), threads(), kill(false) {
		useAll();
	}

	worker::jobPtr worker::add(
		method m, object& o, varList args) {
		auto j = make_shared<job>(m, o, args);
		mtx.lock();
		jobs.push_back(j);
		mtx.unlock();
		return j;
	}

	void worker::wait() {
		auto cores = thread::hardware_concurrency();
		if (cores != 0 && threads.size() >= cores) {
			while (true) {
				if (mtx.try_lock()) {
					if (jobs.size() == 0) break;
					mtx.unlock();
				}
			}
			mtx.unlock();
		} else {
			kill = true;
			workerProcess(this);
		}
	}

	void worker::killAll() {
		auto cores = thread::hardware_concurrency();
		if (cores != 0 && threads.size() >= cores) {
			kill = true;
			for (auto it = threads.begin(); it != threads.end(); ++it)
				it->join();
			kill = false;
		}
	}

	void worker::useAll() {
		for (uint32_t i = 0; i < thread::hardware_concurrency(); ++i)
			threads.push_back(thread(workerProcess, this));
	}

	void worker::clear() {
		mtx.lock();
		jobs.clear();
		mtx.unlock();
	}
}  // namespace gold