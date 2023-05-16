
#include "../byte_array.h"
namespace star {
	byte_array::byte_array(char* from, unsigned int len, bool NeedCopy = true) {
		if (NeedCopy) {
			this->from = new char[len];
			this->len = len;
			memcpy(this->from, from, len);
		}
		else {
			this->from = from;
			this->len = len;
		}
	}

	byte_array::byte_array(unsigned int len)
	{
		this->from = new char[len];
		memset(this->from, 0, len);
		this->len = len;
	}

	byte_array::byte_array(const byte_array& CopySource)
	{
		this->from = new char[CopySource.len];
		memcpy(this->from, CopySource.from, CopySource.len);
		this->len = CopySource.len;
	}

	byte_array::byte_array(byte_array&& MoveSource) noexcept
	{
		this->from = MoveSource.from;
		this->len = MoveSource.len;
		MoveSource.from = nullptr;
	}

	template<std::size_t Size>
	byte_array::byte_array(char(&from)[Size])
	{
		this->from = new char[Size];
		memcpy(this->from, from, sizeof(char) * Size);
		this->len = Size;
	}

	byte_array& byte_array::operator=(const byte_array& CopySource)
	{
		if (this->from != nullptr) {
			delete[] this->from;	//消除原来的动态数组
		}
		this->from = new char[CopySource.len];
		memcpy(this->from, CopySource.from, CopySource.len);
		this->len = CopySource.len;
		return *this;
	}

	byte_array& byte_array::operator=(byte_array&& MoveSource) noexcept
	{
		if (this->from != nullptr) {
			delete[] this->from;	//消除原来的动态数组
		}
		this->from = MoveSource.from;
		MoveSource.from = nullptr;
		this->len = MoveSource.len;
		return *this;
	}

	byte_array::operator char* () const
	{
		return this->from;
	}

	char& byte_array::operator[](unsigned int index)
	{
		if (index < this->len && index >= 0)
			return *(this->from + index);
		else
			return *(this->from);
	}

	byte_array::byte_array()
	{
		this->from = nullptr;
		this->len = 0;
	}

	byte_array::~byte_array()
	{
		if (this->from != nullptr) {
			delete[] this->from;
		}
	}

	unsigned int byte_array::getLength()
	{
		return this->len;
	}

	char* byte_array::getbytes()
	{
		return this->from;
	}

	byte_builder::byte_element::byte_element(char* from, unsigned int len, bool NeedCopy = true)
		:next(nullptr)
	{
		if (NeedCopy) {
			this->from = new char[len];
			this->len = len;
			memcpy(this->from, from, len);
		}
		else {
			this->from = from;
			this->len = len;
		}
	}

	byte_builder::byte_element::byte_element(const char* from, unsigned int len)
		:next(nullptr)
	{
		this->from = new char[len];
		this->len = len;
		memcpy(this->from, from, len);
	}

	byte_builder::byte_element::byte_element() {
		this->from = nullptr;
		this->len = 0;
		this->next = nullptr;
	}

	byte_builder::byte_element::~byte_element() {
		delete[] from;
	}

	void byte_builder::Recursive_Clear(byte_element* deleteFrom) {
		if (deleteFrom->next != nullptr) {
			Recursive_Clear(deleteFrom->next);
		}
		delete deleteFrom->next;
	}

	void byte_builder::Recursive_Collect(byte_element* CopyFrom, char* Collection, int offset = 0) {
		if (CopyFrom->next != nullptr) {
			memcpy(Collection + offset, CopyFrom->next->from, CopyFrom->next->len);
			Recursive_Collect(CopyFrom->next, Collection, offset + CopyFrom->next->len);
		}
		delete CopyFrom->next;
	}

	byte_builder::byte_builder() {
		this->head = new byte_element();
		this->tail = this->head;
		this->len = 0;
	}

	byte_builder::byte_builder(char* from, unsigned int len, bool NeedCopy = true) {
		this->head = new byte_element();
		byte_element* create = new byte_element(from, len, NeedCopy);
		this->tail->next = create;
		this->tail = create;
		this->len = len;
	}

	byte_builder::~byte_builder() {
		this->clear();
		delete this->head;
	}

	void byte_builder::pusb_back(const char* from, unsigned int len)
	{
		byte_element* create = new byte_element(from, len);
		this->tail->next = create;
		this->tail = create;
		this->len += len;
	}

	void byte_builder::pusb_back(char* from, unsigned int len, bool NeedCopy = true) {
		byte_element* create = new byte_element(from, len, NeedCopy);
		this->tail->next = create;
		this->tail = create;
		this->len += len;
	}

	byte_array byte_builder::getArray() {
		this->collect();
		if (this->head->next == nullptr) {
			return byte_array();
		}
		else {
			return byte_array(this->head->next->from, this->len);
		}
	}


	unsigned int byte_builder::getLength() {
		return this->len;
	}

	void byte_builder::collect() {
		if (this->head->next != nullptr && this->head->next->next != nullptr) {
			char* Collection = new char[this->len];
			this->Recursive_Collect(this->head, Collection);
			byte_element* create = new byte_element(Collection, this->len, false);
			this->head->next = create;
			this->tail = create;
		}
	}

	void byte_builder::clear() {
		Recursive_Clear(this->head);
		this->head->next = nullptr;
		this->tail = this->head;
		this->len = 0;
	}
}
