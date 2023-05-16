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
		std::condition_variable condition_lock;
		block_cin():std::stringstream{}{}
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

	std::once_flag block_cin::call_flag{};
	block_cin block_cin::cin{};
	block_cin& cin{ block_cin::cin };
	std::shared_ptr<istream::cin_thread> istream::handle{nullptr};

	

	template<typename T>
	inline block_cin& block_cin::operator>>(T& buffer)
	{
		std::call_once(block_cin::call_flag, istream::initialization);
		if (this->rdbuf()->in_avail() <= 0) {
			std::unique_lock lock_t(this->locker);
			this->condition_lock.wait(lock_t, [&]() {return this->rdbuf()->in_avail() > 0 || this_thread::interrupt_ptr_local->load(); });
		}
		std::stringstream& base = *this;
		base >> buffer;
		return *this;
	}
}