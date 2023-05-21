#pragma once
#ifdef _WIN32							//Windowsͷ�ļ�
#pragma comment(lib,"ws2_32.lib")		//���Ӷ�̬��
#ifndef _WINSOCKAPI_					//��ֹ�ظ�����ͷ�ļ�
#include <Winsock2.h>  
#include<WS2tcpip.h>
#endif
#endif

#include<atomic>
#include<mutex>
#include<string>
#include<string.h>
#include<iostream>
namespace star {
	enum class ip_type :char {	//ip��Э������
		ipv4,
		ipv6
	};
	enum class tran_type :char {	//�����Э��������
		udp,
		tcp
	};
	class net_exception :public std::exception {
	private:
		std::string error_msg;
	public:
		net_exception(std::string error_msg);
		virtual const char* what() const throw() override;
	};
	union socket_addr {
		sockaddr_in ipv4;
		sockaddr_in6 ipv6;
	};
}