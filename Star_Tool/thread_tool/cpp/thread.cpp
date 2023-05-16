
#ifndef STAR_THREAD_CPP_REX
#define STAR_THREAD_CPP_REX
#endif // !STAR_THREAD_CPP_REX
#include<atomic>
#include<memory>
namespace star {
	class interrupt_except;
	namespace this_thread {
		thread_local std::shared_ptr<interrupt_except> interrupt_msg_ptr_local = std::make_shared<interrupt_except>();
		thread_local std::shared_ptr<std::atomic<bool>> interrupt_ptr_local = std::make_shared<std::atomic<bool>>(false);
	}
}
#include"../thread.h"
namespace star {
	interrupt_except::interrupt_except()
		:std::exception(), error_msg("")
	{}

	void interrupt_except::set_msg(std::string& msg)
	{
		error_msg.append("interrupt_except_error:");
		error_msg.append(msg);
	}

	const char* interrupt_except::what() const throw()
	{
		return error_msg.c_str();
	}
	
	namespace this_thread {
		void interrupt() {
			interrupt_ptr_local->store(true, std::memory_order_release);
		}
		void interrupt(std::string msg)
		{
			interrupt_msg_ptr_local->set_msg(msg);
			interrupt_ptr_local->store(true,std::memory_order_release);
		}
		void try_interrepted() noexcept(false)
		{
			if (interrupt_ptr_local->load(std::memory_order_acquire)) {
				throw* (interrupt_msg_ptr_local);
			}
		}
	}

	void thread::interrupt() {
		interrupt_ptr->store(true, std::memory_order_release);
	}

	void thread::interrupt(std::string msg)
	{
		this->interrupt_msg_ptr->set_msg(msg);
		interrupt_ptr->store(true, std::memory_order_release);
	}

	void thread::try_interrepted()  noexcept(false)
	{
		if (this->interrupt_ptr->load(std::memory_order_acquire)) {
			throw* (this->interrupt_msg_ptr);
		}
	}
	
}
