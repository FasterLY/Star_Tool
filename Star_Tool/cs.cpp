#include <iostream>
using namespace std;
#include"tool/coroutineHandle.h"
class MyAwaiter :star::awaiter {
public:
    //Э�̳�ʼ�����
    virtual bool await_ready() {
        cout << __func__ << endl;
        return false;
    }
    //Э����ͣ
    virtual void await_suspend(std::coroutine_handle<> h) {
        cout << __func__ << endl;
    }
    //Э�ָ̻�
    virtual void await_resume() {
        cout << __func__ << endl;
    }
};

star::co_handle<bool> funcMachine() {
    co_yield true;
    co_yield true;
    cout << "before resume" << endl;
    co_await star::resume_waiter([]() {cout << "resume" << endl; });
    cout << "after resume" << endl;
    co_return;
}

star::co_handle<void> voidMachine() {
    cout << "before resume" << endl;
    co_yield nullptr;
    cout << "after resume" << endl;
    co_return;
}

star::co_handle<bool, true, true> returnMachine() {
    co_yield true;
    co_yield true;
    cout << "before resume" << endl;
    co_await star::resume_waiter([]() {cout << "resume" << endl; });
    cout << "after resume" << endl;
    co_return false;
}

int main()
{
    //star::coroutineHandle<int> ranger = numMachine(3);
    //star::co_handle<bool> funcInstance = funcMachine();
    auto suspendInstance = returnMachine();
    int i = 1;

    while (!suspendInstance.isEnd()) {
        cout << i++ << " ";
        suspendInstance.resume();
        cout << endl << suspendInstance.getIterator().operator*();
    }
    cout << endl << suspendInstance.getIterator().operator*();
}