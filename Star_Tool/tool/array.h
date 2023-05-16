#pragma once
namespace star {
	template<class T>
	class array {
	private:
		T* from;
		unsigned int len;
	public:
		/*
			����������ã�
				from		��������ͷָ���ַ
				len			�������鳤��
				NeedCopy	�Ƿ����鿽�����ݣ��������ͷָ��Ϊ��̬����ͷָ�룬��Ӧ����Ϊfalse
		*/
		array(T* from, unsigned int len, bool NeedCopy);		//��׼���캯��
		array(unsigned int len);
		array(const array<T>& CopySource);					//�������캯��
		array(array<T>&& MoveSource) noexcept;				//�ƶ����캯��
		array<T>& operator=(const array<T>& CopySource);		//������ֵ����
		array<T>& operator=(array<T>&& MoveSource) noexcept;	//�ƶ���ֵ����
		T* begin() {
			return this->from;
		}
		T* end() {
			return this->from + this->len;
		}
		T& operator [](unsigned int index);								//����ʹ�÷���
		array();												//�����鹹�캯��
		~array();
		virtual unsigned int getLength();										//ȡ�����鳤��
		T* getArray();											//ȡ������ͷָ��
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
			delete[] this->from;	//����ԭ���Ķ�̬����
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
			delete[] this->from;	//����ԭ���Ķ�̬����
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

