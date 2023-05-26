#pragma once
#include<iostream>
#include<string>
#include<atomic>
#include<thread>
#include<sstream>
#include<mutex>
#include<condition_variable>
#include"thread.h"
namespace star {
	class block_cin :public std::stringstream {
	private:
		std::mutex locker;
		block_cin():std::stringstream{}{}
		std::shared_ptr<std::condition_variable> condition_lock;
	public:
		block_cin(const block_cin&) = delete;
		block_cin(block_cin&&) = delete;
		void notify();
		template<typename T>
		block_cin& operator >>(T& buffer);
		static block_cin cin;
		static std::once_flag call_flag;
	};
	class istream {
	private:
		class cin_thread {
		private:
			std::thread thread_handle;
			std::atomic<bool> finish_flag;
			std::stringstream& istream_handle;
		public:
			cin_thread(std::stringstream& istream_handle);
			~cin_thread();
			void finish();
			void async_task();
		};
		static std::shared_ptr<cin_thread> handle;
	public:
		static void initialization();
		istream() = delete;
		istream(const istream&) = delete;
		istream(istream&&) = delete;
	};

	extern block_cin& cin;

	template<typename T>
	inline block_cin& block_cin::operator>>(T& buffer)
	{
		std::call_once(block_cin::call_flag, istream::initialization);
		if (this->str().length() - this->tellg() <= 0) {
			std::unique_lock lock_t(this->locker);
			condition_lock = this_thread::condition_lock;
			block_cin& block_this{ *this };
			this->condition_lock->wait(lock_t, [&]() {return block_this.str().length() - block_this.tellg() > 0 || this_thread::interrupt_ptr_local->load(); });
		}
		std::stringstream::operator>>(buffer);
		this->ignore();
		return *this;
	}
}