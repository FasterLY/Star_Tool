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
		* thread_size:�̳߳��߳�������С
		* ���飺����������CPU�߳�����
		*/
		thread_pool(int thread_size);
		/*
		* ��������
		* �Ὣ����ֱ����ǰ�̳߳�����������ִ����ϲŻ᷵��
		* ����ע�⣺�ǵ�ǰ���񣬶���������е�����������������ܻ��в�������û�б�ִ��
		*/
		~thread_pool();
		/*
		*  ������������������񣬷�������ķ���ֵfuture
		*/
		template<typename Func, typename... Args>
		std::future<typename std::invoke_result<Func,Args...>::type> submit(Func&& func, Args&&... args);	
		/*
		* �����̳߳صĴ�С����ȴ��̳߳ص�ǰ������ɲŻ�����ж�
		*/
		void reserver(int thread_size);																		
		/*
		* �̳߳ص�����ͬ��ָ�����ֱ������������е����񶼱�����Ż᷵��
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