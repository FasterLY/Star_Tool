
#include"../thread_pool.h"
namespace star {
	thread_pool::thread_pool(int thread_size)
		:thread_size(thread_size), real_size(thread_size), interrupt_flag(false), thread_id_travel(1)
	{
		for (int i = 0; i < thread_size; i++) {
			thread_map.try_emplace(thread_id_travel, &thread_pool::run_task, this, thread_id_travel);
			thread_runable_map.insert_or_assign(thread_id_travel, true);
			thread_id_travel++;
		}
	}

	thread_pool::~thread_pool()
	{
		this->thread_size.store(0);
		this->interrupt_flag.store(true);
		wait_lock_condition.notify_all();
		for (auto& thread_travel : thread_map) {
			if(thread_travel.second.joinable())
				thread_travel.second.join();
		}
	}

	void thread_pool::run_task(int thread_id)
	{
		while (true) {
			if (interrupt_flag.load()) {
				if (real_size.load() > thread_size.load()) {
					int old_size = real_size.load();
					int next_size = real_size.load() - 1;
					while (!real_size.compare_exchange_strong(old_size, next_size)) {
						if (real_size.load() > thread_size.load()) {
							unsigned int next_size = real_size.load() - 1;
						}
						else {
							goto run_ok;
						}
					}
					{
						std::lock_guard lock(this->map_lock);
						thread_runable_map[thread_id] = false;
					}
					return;
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

	void thread_pool::reserver(int thread_size)
	{
		int old_thread_size = this->thread_size.load(std::memory_order_acquire);
		if (thread_size > old_thread_size) {
			for (int i = 0; i < thread_size - old_thread_size; i++) {
				thread_map.try_emplace(thread_id_travel, &thread_pool::run_task, this, thread_id_travel);
				thread_runable_map.insert_or_assign(thread_id_travel, true);
				thread_id_travel++;
			}
			real_size.store(thread_size, std::memory_order_release);
			this->thread_size.store(thread_size, std::memory_order_release);
		}
		else if (thread_size < old_thread_size) {
			this->thread_size.store(thread_size);
			this->interrupt_flag.store(true);
			wait_lock_condition.notify_all();
			while (old_thread_size > thread_size) {
				for (auto runable_travel = thread_runable_map.begin(); runable_travel != thread_runable_map.end();) {
					auto runable_travel_next = runable_travel;
					runable_travel_next++;
					if (!runable_travel->second) {
						std::lock_guard lock(this->map_lock);
						thread_map[runable_travel->first].detach();
						thread_map.erase(runable_travel->first);
						thread_runable_map.erase(runable_travel->first);
						old_thread_size--;
					}
					runable_travel = runable_travel_next;
					std::this_thread::yield();
				}
				std::this_thread::yield();
			}
			this->interrupt_flag.store(false);				//恢复中断标志位，让其他线程正常执行
		}
	}

	void thread_pool::join()
	{
		std::unique_lock task_lock(this->task_over);
		while (!task_queue.empty())
			task_over_condition.wait(task_lock);
		this->thread_size.store(0);
		this->interrupt_flag.store(true);
		wait_lock_condition.notify_all();
		for (auto& thread_travel: thread_map) {
			if (thread_travel.second.joinable())
				thread_travel.second.join();
		}
		thread_map.clear();
		thread_runable_map.clear();
		thread_id_travel = 1;
	}
}