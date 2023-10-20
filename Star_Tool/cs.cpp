#include"tool/co_handle.h"

star::co_handle<int> funcMachine() {
    //每次返回一个值
    for (int i = 0; i < 10; i++)
        co_yield i;
    //稍微调皮一下，我们在这个地方加入了一个co_await，也就是说，我将在这里挂起这个协程而不返回数值。
    co_await star::awaiter();
    //规范起见，由于我的co_handle默认的是不会设置返回值的，故直接return；
    co_return;
    //当然，上述的co_return 是可有可无的，因为只要协程函数执行完毕，就会自动调用co_return；
}

star::co_handle<int,star::has_return> funcMachine2() {
	//每次返回一个值
	for (int i = 0; i < 10; i++)
		co_yield i;
	//稍微调皮一下，我们在这个地方加入了一个co_await，也就是说，我将在这里挂起这个协程而不返回数值。
	co_await star::awaiter();
	//规范起见，由于我的co_handle默认的是不会设置返回值的，故直接return；
	co_return 10;
	//当然，上述的co_return 是可有可无的，因为只要协程函数执行完毕，就会自动调用co_return；
}

#include<mutex>
#include <iostream>
#include<thread>
#include<condition_variable>
#include"thread_tool/spin_lock.h"
using namespace std;
star::spin_lock spin;
condition_variable_any cv;
int main()
{
	thread th1([]() {
		spin.lock();
		cout << "th1 获得锁" << endl;
		this_thread::sleep_for(chrono::seconds(1));
		cv.wait(spin);
		cout << "th1 重新获得锁" << endl;
		});
	this_thread::sleep_for(chrono::seconds(1));
	{
		std::lock_guard<star::spin_lock> lock(spin);
		cout << "main 获得锁" << endl;
	}
	cv.notify_all();
	th1.join();
}