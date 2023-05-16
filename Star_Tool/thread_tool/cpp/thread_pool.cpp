
#include"../thread_pool.h"
namespace star {
	inline thread_pool::thread_pool(unsigned int thread_size) :thread_size(thread_size), real_size(thread_size), interrupt_flag(false)
	{
		for (int i = 0; i < thread_size; i++) {
			thread_vec.emplace_back(&thread_pool::run_task, this);
		}
	}

	inline thread_pool::~thread_pool()
	{
		this->thread_size.store(0);
		this->interrupt_flag.store(true);
		wait_lock_condition.notify_all();
		for (int i = 0; i < thread_vec.size(); i++) {
			thread_vec[i].join();
		}
	}

	inline void thread_pool::run_task()
	{
		while (true) {
			if (interrupt_flag.load()) {
				if (real_size.load() > thread_size.load()) {
					unsigned int old_size = real_size.load();
					unsigned int next_size = real_size.load() - 1;
					while (!real_size.compare_exchange_strong(old_size, next_size)) {
						if (real_size.load() > thread_size.load()) {
							unsigned int next_size = real_size.load() - 1;
						}
						else {
							goto run_ok;
						}
					}
					break;
				}
			}
			run_ok:
			std::shared_ptr<std::function<void()>> task = task_queue.try_pop();
			if (task != nullptr) {
				task_over_condition.notify_one();
				(*task)();
			}
			else {
				std::unique_lock lock_gud(this->wait_lock);
				wait_lock_condition.wait(lock_gud);
			}
		}
	}

	inline void thread_pool::reserver(unsigned int thread_size)
	{

	}

	inline void thread_pool::join()
	{
		std::unique_lock task_lock(this->task_over);
		while (!task_queue.empty())
			task_over_condition.wait(task_lock);
		this->thread_size.store(0);
		this->interrupt_flag.store(true);
		wait_lock_condition.notify_all();
		for (int i = 0; i < thread_vec.size(); i++) {
			thread_vec[i].join();
		}
		thread_vec.clear();
	}
}