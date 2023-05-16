#ifndef CS_CPP
#define CS_CPP
#endif // !CS_CPP

#include<iostream>
namespace star_cs {
	void func() {
		std::cout << "func test" << std::endl;
	}
}

namespace star {

	namespace ch {
		thread_local int i = 99;
		void func_cs() {
			std::cout << i << std::endl;
		}
	}
}