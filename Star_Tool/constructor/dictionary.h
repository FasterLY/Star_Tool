#pragma once
#include<string>
#include<map>
namespace star {
	struct class_base {
		using type = class_base;
		
	};
	class dictionary {
	private:
		std::map<std::string, class_base*> maps;
	public:

	};
}