#pragma once
#include<atomic>
namespace star {
	template<typename T>
	class weak_ptr;
	template<typename T>
	class shared_ptr {
	public:
		shared_ptr();
		shared_ptr(T* data);
		shared_ptr(const shared_ptr<T>& CopySource);
		shared_ptr(shared_ptr<T>&& MoveSource);
		~shared_ptr();
		T* operator ->();
		T& operator *();
		shared_ptr<T>& operator = (const shared_ptr<T>& CopySource);
		shared_ptr<T>& operator = (shared_ptr<T>&& MoveSource);
	private:
		std::atomic<unsigned int>* strong_ref;
		std::atomic<unsigned int>* weak_ref;
		T* ptr_data;
		friend class weak_ptr<T>;
	};

	template<typename T>
	inline shared_ptr<T>::shared_ptr()
		:strong_ref(nullptr), weak_ref(nullptr), ptr_data(nullptr)
	{

	}

	template<typename T>
	inline shared_ptr<T>::shared_ptr(T* ptr_data)
		: strong_ref(new std::atomic<unsigned int>(1)), weak_ref(new std::atomic<unsigned int>(0)),
		ptr_data(ptr_data)
	{

	}

	template<typename T>
	inline shared_ptr<T>::shared_ptr(const shared_ptr<T>& CopySource)
		: strong_ref(CopySource.strong_ref), weak_ref(CopySource.weak_ref),
		ptr_data(CopySource.ptr_data)
	{
		++(*(this->strong_ref));
		/*	//无差错等效方法：
		int destination = strong_ref->load(std::memory_order_acquire);
		while (!strong_ref->compare_exchange_weak(destination, destination + 1));
		*/
	}

	template<typename T>
	inline shared_ptr<T>::shared_ptr(shared_ptr<T>&& MoveSource)
		: strong_ref(MoveSource.strong_ref), weak_ref(MoveSource.weak_ref),
		ptr_data(MoveSource.ptr_data)
	{
		MoveSource.ptr_data = nullptr;
		MoveSource.strong_ref = nullptr;
		MoveSource.weak_ref = nullptr;
	}

	template<typename T>
	inline shared_ptr<T>::~shared_ptr()
	{
		(*(this->strong_ref))--;
		if (this->strong_ref->load(std::memory_order_acquire) == 0) {
			delete this->ptr_data;
			if (this->weak_ref->load(std::memory_order_acquire) == 0) {
				delete this->strong_ref;
				delete this->weak_ref;
			}
		}
		
	}

	template<typename T>
	inline T* shared_ptr<T>::operator->()
	{
		return this->ptr_data;
	}

	template<typename T>
	inline T& shared_ptr<T>::operator*()
	{
		return *(this->ptr_data);
	}

	template<typename T> 
	inline shared_ptr<T>& shared_ptr<T>::operator=(const shared_ptr<T>& CopySource)
	{
		if (this->ptr_data != nullptr) {
			(*(this->strong_ref))--;
			if (this->strong_ref->load(std::memory_order_acquire) == 0) {
				delete this->ptr_data;
			}
			if (this->weak_ref->load(std::memory_order_acquire) == 0) {
				delete this->strong_ref;
				delete this->weak_ref;
			}
		}
		++(*(CopySource.strong_ref));
		this->strong_ref = CopySource.strong_ref;
		this->weak_ref = CopySource.weak_ref;
		this->ptr_data = CopySource.ptr_data;
		return *this;
	}

	template<typename T>
	inline shared_ptr<T>& shared_ptr<T>::operator=(shared_ptr<T>&& MoveSource)
	{
		if (this->ptr_data != nullptr) {
			(*(this->strong_ref))--;
			if (this->strong_ref->load(std::memory_order_acquire) == 0) {
				delete this->ptr_data;
			}
			if (this->weak_ref->load(std::memory_order_acquire) == 0) {
				delete this->strong_ref;
				delete this->weak_ref;
			}
		}
		this->strong_ref = MoveSource.strong_ref;
		this->weak_ref = MoveSource.weak_ref;
		this->ptr_data = MoveSource.ptr_data;
		MoveSource.ptr_data = nullptr;
		MoveSource.strong_ref = nullptr;
		MoveSource.weak_ref = nullptr;
		return *this;
	}
}