#pragma once

#include <mutex>
#include <thread>

#include "types.hpp"

namespace gold {
	struct worker {
	 protected:
		struct job;
		typedef shared_ptr<worker::job> jobPtr;
		vector<jobPtr> jobs;
		vector<thread> threads;
		bool kill;
		mutex mtx;

		static int workerProcess(worker*);

		jobPtr nextJob();
		bool shouldDie();

	 public:
		worker();

		jobPtr add(const method& m, object& o, const list& args);
		void wait();
		void clear();
		void killAll();
		void useAll();
	};
}  // namespace gold