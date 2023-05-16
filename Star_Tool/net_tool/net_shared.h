#pragma once
#include<atomic>
#include<string>
#include<string.h>
#include<iostream>
namespace star {
	enum class ip_type :char {	//ip族协议类型
		ipv4,
		ipv6
	};
	enum class tran_type :char {	//传输层协议流类型
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
}

#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")		//链接动态库
#ifndef _WINSOCKAPI_
#include <Winsock2.h>  
#include<WS2tcpip.h>
#endif
#elif  __linux__

#endif // WIN32