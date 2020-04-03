#pragma once

#include <thread>
#include <mutex>
#include "object.hpp"
#include "var.hpp"

namespace red {
	class worker {
		protected:
			class job;
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

		jobPtr add(method m, object& o, var args);
		void wait();
		void clear();
		void killAll();
		void useAll();
	};
}  // namespace red