#pragma once
#include<iostream>
namespace star_cs {
	void func();
}

namespace star {
	namespace ch {
#ifndef CS_CPP
		extern thread_local int i;
#endif // !CS_CPP
		void func_cs();
	}

}