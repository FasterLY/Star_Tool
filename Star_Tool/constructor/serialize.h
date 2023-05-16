#pragma once
#include<iostream>
namespace star {
	class serializable
	{
	public:
		serializable() {};
		~serializable() {};
		virtual std::size_t serialize(char* buf, std::size_t&& off = 0) = 0;
		virtual serializable serialize(char* buf, std::size_t&& len, std::size_t&& off = 0) = 0;
	};
}