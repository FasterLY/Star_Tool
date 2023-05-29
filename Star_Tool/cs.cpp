#include<iostream>
#include<vector>
#include"net_tool/tcp_socket.h"
#include"tool/array.h"
#include"tool/hash_val.h"
#ifdef _WIN32
#include<Windows.h>
#endif // WIN32

using namespace std;
class Obj_cs {
public:
	Obj_cs(int i) {
		//memset(this, 0, sizeof(decltype(*this)));
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
	Obj_cs2() :Obj_cs(0) {
		//memset(this, 0, sizeof(decltype(*this)));
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
#include"safe_container/no_mutex/queue.h"
#include"tool/get_set.h"
namespace star {
	extern int i;
}

class MyClass
{
public:
	MyClass() :buf { 0 }
	{}
	~MyClass() = default;
	star::get_set<int> Buf{ 
		//data:
		this->buf,
		//getter:
		[](int i) {
			return i;
		},
		//setter:
		[](int i) {
			return i > 99 ? 99 : i;
		} 
	};
private:
	int buf;
};
#include"thread_tool/thread_pool.h"
#include"thread_tool/thread.h"
#include"thread_tool/interrupt_cin.h"
int main() {
	char msg[] = "hello linux";
	star::tcp_socket_server server_socket(8888);
	server_socket.freeAddr();
	star::tcp_socket client_socket = server_socket.accept();
	char buffer[1024];
	client_socket.read(buffer, 1024);
	cout << buffer;
	client_socket.write(msg, sizeof(msg));
}