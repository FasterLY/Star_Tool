#pragma once
#include<atomic>
#include<mutex>
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
namespace star {
	union socket_addr {
		sockaddr_in ipv4;
		sockaddr_in6 ipv6;
	};
	void net_Initialize();
#ifndef NET_SHARED_CPP
	extern std::once_flag net_Initialize_flag;
	extern std::atomic<bool> isInitialize;
#endif
}
#endif


#elif  __linux__
namespace star {
	union socket_addr {
		sockaddr_in ipv4;
		sockaddr_in6 ipv6;
	};
}
#endif // WIN32