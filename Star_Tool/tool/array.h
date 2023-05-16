#pragma once
namespace star {
	template<class T>
	class array {
	private:
		T* from;
		unsigned int len;
	public:
		/*
			输入参数设置：
				from		传入数组头指针地址
				len			传入数组长度
				NeedCopy	是否将数组拷贝备份，若传入的头指针为动态数组头指针，则应该置为false
		*/
		array(T* from, unsigned int len, bool NeedCopy);		//标准构造函数
		array(unsigned int len);
		array(const array<T>& CopySource);					//拷贝构造函数
		array(array<T>&& MoveSource) noexcept;				//移动构造函数
		array<T>& operator=(const array<T>& CopySource);		//拷贝赋值函数
		array<T>& operator=(array<T>&& MoveSource) noexcept;	//移动赋值函数
		T* begin() {
			return this->from;
		}
		T* end() {
			return this->from + this->len;
		}
		T& operator [](unsigned int index);								//数组使用符号
		array();												//空数组构造函数
		~array();
		virtual unsigned int getLength();										//取得数组长度
		T* getArray();											//取得数组头指针
	};

	template<class T>
	array<T>::array(T* from, unsigned int len, bool NeedCopy)
	{
		if (NeedCopy) {
			this->from = new T[len];
			this->len = len;
			memcpy(this->from, from, len * sizeof(T));
		}
		else {
			this->from = from;
			this->len = len;
		}
	}

	template<class T>
	array<T>::array(unsigned int len)
	{
		this->from = new T[len]{};
		memset(this->from, 0, len);
		this->len = len;
	}

	template<class T>
	array<T>::array(const array<T>& CopySource)
	{
		this->from = new T[CopySource.len];
		memcpy(this->from, CopySource.from, CopySource.len * sizeof(T));
		this->len = CopySource.len;
	}

	template<class T>
	array<T>::array(array&& MoveSource) noexcept
	{
		this->from = MoveSource.from;
		this->len = MoveSource.len;
		MoveSource.from = nullptr;
	}

	template<class T>
	array<T>& array<T>::operator=(const array& CopySource)
	{
		if (this->from != nullptr) {
			delete[] this->from;	//消除原来的动态数组
		}
		this->from = new T[CopySource.len];
		memcpy(this->from, CopySource.from, CopySource.len * sizeof(T));
		this->len = CopySource.len;
		return *this;
	}

	template<class T>
	array<T>& array<T>::operator=(array&& MoveSource) noexcept
	{
		if (this->from != nullptr) {
			delete[] this->from;	//消除原来的动态数组
		}
		this->from = MoveSource.from;
		MoveSource.from = nullptr;
		this->len = MoveSource.len;
		return *this;
	}

	template<class T>
	T& array<T>::operator[](unsigned int index)
	{
		if (index < this->len && index >= 0)
			return *(this->from + index);
		else
			return *(this->from);
	}

	template<class T>
	array<T>::array()
	{
		this->from = nullptr;
		this->len = 0;
	}

	template<class T>
	array<T>::~array()
	{
		if (this->from != nullptr) {
			delete[] this->from;
		}
	}

	template<class T>
	unsigned int array<T>::getLength()
	{
		return this->len;
	}

	template<class T>
	T* array<T>::getArray()
	{
		return this->from;
	}
}

