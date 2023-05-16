#include<iostream>
#include<vector>
#include"tool/array.h"
#include"tool/hash_val.h"
#ifdef _WIN32
#include<Windows.h>
#endif // WIN32

using namespace std;
class Obj_cs {
public:
	Obj_cs() {
		cout << "Obj_cs is created" << endl;
		this->cs();
	}
	virtual ~Obj_cs() {
		cout << "Obj_cs is deleted" << endl;
	}
	virtual void cs() {
		std::cout << "Obj_cs Base test..." << std::endl;
	}
};

class Obj_cs2 :public Obj_cs {
public:
	Obj_cs2() :Obj_cs() {
		cout << "Obj_cs2 is created" << endl;
		this->cs();
	}
	virtual ~Obj_cs2() override {
		cout << "Obj_cs2 is deleted" << endl;
	}
	virtual void cs() override {
		std::cout << "Obj_cs2 test..." << i << std::endl;
	}
	void func() {
		cout << "Obj_cs2 func " <<i<< endl;
	}
	int i = 999;
};
#include<atomic>
#include<thread>
#include<algorithm>
#include<mutex>
#include"tool/shared_ptr.h"
#include"thread_tool/thread.h"
#include"bytes_tool/byte_array.h"

#include"cs3.h"
namespace star {
	extern int i;
}
int main() {
	
	star::byte_builder build{};
	build.pusb_back("abcdefghij", 10);
	auto func = []() {
		star::this_thread::interrupt("interrupt ok!");
		try {
			star::this_thread::try_interrepted();
		}
		catch (std::exception& err) {
			cout << err.what() << endl;
		}
	};
	star::thread th1(func);
	th1.join();
	


}