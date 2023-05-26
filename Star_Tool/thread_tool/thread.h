#pragma once
#include<atomic>
#include<thread>
#include<functional>
#include<memory>
#include<string>
#include<exception>

extern int i;
namespace star {

	class interrupt_except 
		:public std::exception {
	private:
		std::string error_msg;
	public:
		/*
		* 默认构造函数
		*/
		interrupt_except();
		/*
		* 设置错误信息
		*/
		void set_msg(std::string& msg);
		/*
		* 获取错误信息
		*/
		virtual const char* what() const throw() override;
		/*
		* 设置错误信息
		*/
		void clear();
	};
	namespace this_thread {
#ifndef STAR_THREAD_CPP_REX
		extern thread_local std::shared_ptr<interrupt_except> interrupt_msg_ptr_local;
		extern thread_local std::shared_ptr<std::atomic<bool>> interrupt_ptr_local;
		extern thread_local std::shared_ptr<std::condition_variable> condition_lock;
#endif // !STAR_THREAD_CPP_REX
		/*
		* 在当前线程发送中断消息
		*/
		void interrupt();
		/*
		* 在当前线程发送中断消息并附带错误消息
		*/
		void interrupt(std::string msg);
		/*
		* 尝试中断，若有中断信号则会抛出interrupt_except错误否则正常通过
		*/
		void try_interrepted() noexcept(false);
		/*
		* 清除中断信号和错误消息
		*/
		void clear();
	}
	class thread : public std::thread {
	public:
		template<typename Function, typename... Args>
		thread(Function&& function, Args&&... args);
		template<typename Function>
		thread(Function&& function);
		/*
		* 在当前线程发送中断消息
		*/
		void interrupt();
		/*
		* 在当前线程发送中断消息并附带错误消息
		*/
		void interrupt(std::string msg);
		/*
		* 尝试中断，若有中断信号则会抛出interrupt_except错误否则正常通过
		*/
		void try_interrepted() noexcept(false);
		/*
		* 清除中断信号和错误消息
		*/
		void clear();
	private:
		std::shared_ptr<std::atomic<bool>> interrupt_ptr;
		std::shared_ptr<interrupt_except> interrupt_msg_ptr;
	};

	template<typename Function, typename... Args>
	inline thread::thread(Function&& function, Args&&... args)
		: interrupt_ptr(std::make_shared<std::atomic<bool>>(false)),
		interrupt_msg_ptr(std::make_shared<interrupt_except>()),
		std::thread([&]() {star::this_thread::interrupt_ptr_local = this->interrupt_ptr;  star::this_thread::interrupt_msg_ptr_local = this->interrupt_msg_ptr; std::bind(function, args...)(); })
	{} 

	template<typename Function>
	inline thread::thread(Function&& function)
		: interrupt_ptr(std::make_shared<std::atomic<bool>>(false)),
		interrupt_msg_ptr(std::make_shared<interrupt_except>()),
		std::thread([&]() {star::this_thread::interrupt_ptr_local = this->interrupt_ptr; star::this_thread::interrupt_msg_ptr_local = this->interrupt_msg_ptr; std::bind(function)(); })
	{}

	
}
