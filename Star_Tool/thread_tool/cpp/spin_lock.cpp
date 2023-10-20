#include "../spin_lock.h"
namespace star {
	spin_lock::spin_lock() {
		lock_flag.store(false, std::memory_order_release);
	}
	spin_lock::~spin_lock() {
		lock_flag.store(false, std::memory_order_release);
	}
	void spin_lock::lock()
	{
		bool expected = false;
		while (!lock_flag.compare_exchange_weak(expected, true, std::memory_order_acquire, std::memory_order_relaxed))
		{
			expected = false;
		}
	}
	void spin_lock::unlock()
	{
		lock_flag.store(false, std::memory_order_release);
	}

	bool spin_lock::try_lock()
	{
		bool expected = false;
		return lock_flag.compare_exchange_strong(expected, true, std::memory_order_acquire, std::memory_order_relaxed);
	}
}