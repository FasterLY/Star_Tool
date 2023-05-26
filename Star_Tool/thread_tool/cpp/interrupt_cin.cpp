#include"../interrupt_cin.h"
namespace star {
	std::once_flag block_cin::call_flag{};
	block_cin block_cin::cin{};
	block_cin& cin{ block_cin::cin };
	std::shared_ptr<istream::cin_thread> istream::handle{ nullptr };
	void block_cin::notify() {
		this->condition_lock->notify_all();
	}
	istream::cin_thread::cin_thread(std::stringstream& istream_handle) :finish_flag(true), istream_handle(istream_handle) {
		thread_handle = std::thread(&istream::cin_thread::async_task,this);
	}

	istream::cin_thread::~cin_thread() {
		this->finish();
		if (this->thread_handle.joinable())
			this->thread_handle.join();
	}

	void istream::cin_thread::finish() {
		finish_flag.store(false);
		cin.notify();
	}

	void istream::cin_thread::async_task() {
		while (this->finish_flag.load()) {
			char buffer;
			std::cin.get(buffer);
			this->istream_handle << buffer;
			cin.notify();
		}
	}


	void istream::initialization()
	{
		istream::handle = std::make_shared<istream::cin_thread>(star::cin);
	}

}