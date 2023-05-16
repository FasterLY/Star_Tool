#pragma once
#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<future>
#include<vector>
#include<type_traits>
#include<iostream>
#include<atomic>
#include"../safe_container/mutex/queue.h"
namespace star {
	class thread_pool
	{
	public:
		thread_pool(unsigned int thread_size);
		~thread_pool();
		template<typename Func, typename... Args>
		std::future<typename std::invoke_result<Func,Args...>::type> submit(Func&& func, Args&&... args);
		void reserver(unsigned int thread_size);
		void join();
	private:
		std::vector<std::thread> thread_vec;
		std::atomic<unsigned int> thread_size;
		std::atomic<unsigned int> real_size;
		star::safe_container::mutex::queue<std::function<void()>> task_queue;
		std::mutex wait_lock;
		std::condition_variable wait_lock_condition;
		std::mutex task_over;
		std::condition_variable task_over_condition;
		std::atomic<bool> interrupt_flag;
	private:
		void run_task();
	};

	template<typename Func, typename ...Args>
	inline std::future<typename std::invoke_result<Func, Args...>::type> thread_pool::submit(Func&& func, Args&& ...args)
	{
		using return_type = typename std::invoke_result<Func, Args...>::type;
		auto task = std::make_shared< std::packaged_task<return_type()> >(
			std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
			);
		task_queue.emplace([task]() { (*task)(); });
		wait_lock_condition.notify_all();
		return task->get_future();
	}
}