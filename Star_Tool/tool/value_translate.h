#pragma once
#include<iostream>
#include<string>
namespace star {
	template<typename Value_Type>
	concept Translate_Type = std::is_same<typename std::remove_cv<typename std::remove_reference<typename std::make_signed<Value_Type>::type>::type>::type, short>::value ||
		std::is_same<typename std::remove_cv<typename std::remove_reference<typename std::make_signed<Value_Type>::type>::type>::type, int>::value ||
		std::is_same<typename std::remove_cv<typename std::remove_reference<typename std::make_signed<Value_Type>::type>::type>::type, long>::value ||
		std::is_same<typename std::remove_cv<typename std::remove_reference<typename std::make_signed<Value_Type>::type>::type>::type, long long>::value ||
		std::is_same<typename std::remove_cv<typename std::remove_reference<typename std::make_signed<Value_Type>::type>::type>::type, float>::value ||
		std::is_same<typename std::remove_cv<typename std::remove_reference<typename std::make_signed<Value_Type>::type>::type>::type, double>::value;
		
	template<typename Value_Type> requires Translate_Type<Value_Type>
	void Translate(Value_Type&& value, char* buffer, std::size_t&& off = 0)
	{
		char* ptr = (char*)(&value);
		memcpy(buffer + off, ptr, sizeof(value));
	}

	template<typename Value_Type> requires Translate_Type<Value_Type>
	std::unique_ptr<char[]> Translate(Value_Type&& value)
	{
		char* ptr = (char*)(&value);
		std::unique_ptr<char[]> buffer = std::make_unique<char[]>(sizeof(value));
		memcpy(buffer.get(), ptr, sizeof(value));
		return std::move(buffer);
	}

	template<typename Value_Type> requires Translate_Type<Value_Type>
	Value_Type Translate(char* buffer, std::size_t&& off = 0)
	{
		Value_Type value{};
		char* ptr = (char*)(&value);
		memcpy(ptr, buffer + off, sizeof(value));
		return value;
	}


}