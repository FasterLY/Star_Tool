#include"tool/co_handle.h"

star::co_handle<int> funcMachine() {
    //ÿ�η���һ��ֵ
    for (int i = 0; i < 10; i++)
        co_yield i;
    //��΢��Ƥһ�£�����������ط�������һ��co_await��Ҳ����˵���ҽ�������������Э�̶���������ֵ��
    co_await star::awaiter();
    //�淶����������ҵ�co_handleĬ�ϵ��ǲ������÷���ֵ�ģ���ֱ��return��
    co_return;
    //��Ȼ��������co_return �ǿ��п��޵ģ���ΪֻҪЭ�̺���ִ����ϣ��ͻ��Զ�����co_return��
}

star::co_handle<int,star::has_return> funcMachine2() {
	//ÿ�η���һ��ֵ
	for (int i = 0; i < 10; i++)
		co_yield i;
	//��΢��Ƥһ�£�����������ط�������һ��co_await��Ҳ����˵���ҽ�������������Э�̶���������ֵ��
	co_await star::awaiter();
	//�淶����������ҵ�co_handleĬ�ϵ��ǲ������÷���ֵ�ģ���ֱ��return��
	co_return 10;
	//��Ȼ��������co_return �ǿ��п��޵ģ���ΪֻҪЭ�̺���ִ����ϣ��ͻ��Զ�����co_return��
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
		cout << "th1 �����" << endl;
		this_thread::sleep_for(chrono::seconds(1));
		cv.wait(spin);
		cout << "th1 ���»����" << endl;
		});
	this_thread::sleep_for(chrono::seconds(1));
	{
		std::lock_guard<star::spin_lock> lock(spin);
		cout << "main �����" << endl;
	}
	cv.notify_all();
	th1.join();
}