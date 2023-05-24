#pragma once
#include<thread>
#include<mutex>
#include<condition_variable>
#include<unordered_map>
#include<functional>
#include<future>
#include<list>
#include<type_traits>
#include<iostream>
#include<atomic>
#include"../safe_container/mutex/queue.h"
namespace star {
	class thread_pool
	{
	public:
		/*
		* thread_size:线程池线程数量大小
		* 建议：不超过可用CPU线程数量
		*/
		thread_pool(int thread_size);
		/*
		* 析构函数
		* 会将阻塞直到当前线程池内所有任务执行完毕才会返回
		* ！！注意：是当前任务，而非任务队列的所有任务，析构后可能会有部分任务没有被执行
		*/
		~thread_pool();
		/*
		*  向任务队列中添置任务，返回任务的返回值future
		*/
		template<typename Func, typename... Args>
		std::future<typename std::invoke_result<Func,Args...>::type> submit(Func&& func, Args&&... args);	
		/*
		* 重置线程池的大小，会等待线程池当前任务完成才会进行判断
		*/
		void reserver(int thread_size);																		
		/*
		* 线程池的整体同步指令，阻塞直到所有任务队列的任务都被处理才会返回
		*/
		void join();																						
	private:
		std::unordered_map<int, std::thread> thread_map;
		std::unordered_map<int, bool> thread_runable_map;
		int thread_id_travel;
		std::atomic<int> thread_size;
		std::atomic<int> real_size;
		star::safe_container::mutex::queue<std::function<void()>> task_queue;
		std::mutex map_lock;
		std::mutex wait_lock;
		std::condition_variable wait_lock_condition;
		std::mutex task_over;
		std::condition_variable task_over_condition;
		std::atomic<bool> interrupt_flag;
	private:
		void run_task(int thread_id);
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