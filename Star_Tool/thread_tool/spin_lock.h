#pragma once
#include<atomic>
namespace star {
	class spin_lock
	{
	private:
		std::atomic<bool> lock_flag;
	public:
		spin_lock();
		~spin_lock();
		void lock();
		void unlock();
		bool try_lock();
	};
}

