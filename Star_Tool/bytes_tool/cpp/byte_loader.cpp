#include "../byte_loader.h"

namespace star {
	byte_loader::byte_loader(unsigned int size) {
		this->from = new char[size];
		this->size = size;
		this->loaded_size = 0;
		this->loading = this->from;
	}

	byte_loader::~byte_loader() {
		if (this->from != nullptr)
			delete[] from;
	}

	unsigned int byte_loader::load(char* Resource, unsigned int Load_len, unsigned int off = 0)
	{
		Load_len = Load_len > (this->size - this->loaded_size) ? this->size - this->loaded_size : Load_len;
		memcpy(this->loading, Resource + off, Load_len);
		this->loading += Load_len;
		this->loaded_size += Load_len;
		return Load_len;
	}

	byte_array byte_loader::getArray()
	{
		if (this->loaded_size == this->size) {
			char* buf = this->from;
			this->from = nullptr;
			return byte_array(buf, this->size, false);
		}
		else {	//没有装满返回空数组
			return byte_array();
		}
	}

	bool byte_loader::isFull()
	{
		if (this->loaded_size == this->size) {
			return true;
		}
		else {
			return false;
		}
	}

	unsigned int byte_loader::getSize()
	{
		return this->size;
	}

	unsigned int byte_loader::getLoaded()
	{
		return this->loaded_size;
	}
}