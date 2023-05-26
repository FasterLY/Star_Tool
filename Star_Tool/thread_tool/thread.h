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
		* Ĭ�Ϲ��캯��
		*/
		interrupt_except();
		/*
		* ���ô�����Ϣ
		*/
		void set_msg(std::string& msg);
		/*
		* ��ȡ������Ϣ
		*/
		virtual const char* what() const throw() override;
		/*
		* ���ô�����Ϣ
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
		* �ڵ�ǰ�̷߳����ж���Ϣ
		*/
		void interrupt();
		/*
		* �ڵ�ǰ�̷߳����ж���Ϣ������������Ϣ
		*/
		void interrupt(std::string msg);
		/*
		* �����жϣ������ж��ź�����׳�interrupt_except�����������ͨ��
		*/
		void try_interrepted() noexcept(false);
		/*
		* ����ж��źźʹ�����Ϣ
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
		* �ڵ�ǰ�̷߳����ж���Ϣ
		*/
		void interrupt();
		/*
		* �ڵ�ǰ�̷߳����ж���Ϣ������������Ϣ
		*/
		void interrupt(std::string msg);
		/*
		* �����жϣ������ж��ź�����׳�interrupt_except�����������ͨ��
		*/
		void try_interrepted() noexcept(false);
		/*
		* ����ж��źźʹ�����Ϣ
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
