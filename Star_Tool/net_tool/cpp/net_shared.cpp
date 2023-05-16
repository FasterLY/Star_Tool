#include"../net_shared.h"

namespace star {
	net_exception::net_exception(std::string error_msg)
		:std::exception(), error_msg("net_exception_error:"+error_msg)
	{}

	const char* net_exception::what() const throw()
	{
		return error_msg.c_str();
	}
}

#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")		//链接动态库
#ifndef _WINSOCKAPI_
#include <Winsock2.h>  
#include<WS2tcpip.h>
#endif
namespace star {
	std::atomic<bool> isInitialize(false);
	//封装动态链接库的初始化方法
	class Windows_net_Initialize {
	public:
		Windows_net_Initialize() {
			if (isInitialize.load(std::memory_order_acquire)) {
				WORD wVersionRequested;
				WSADATA wsadata;
				int err;
				wVersionRequested = MAKEWORD(1, 1);
				err = WSAStartup(wVersionRequested, &wsadata);
				if (err != 0) {
					std::cout << "WSA net_tool initialize fail!" << std::endl;
					return;
				}
				if (LOBYTE(wsadata.wVersion) != 1 || HIBYTE(wsadata.wVersion) != 1) {
					WSACleanup();
					std::cout << "WSA net_tool initialize fail!" << std::endl;
					return;
				}
				isInitialize.store(true);
				std::cout << "WSA net_tool initialize succeed" << std::endl;
			}
		}
		~Windows_net_Initialize() {
			WSACleanup();
			std::cout << "WSA net_tool uninstall succeed" << std::endl;
		}
	};			//封装动态链接库的初始化方法
	const Windows_net_Initialize windows_net_initialize;		//初始化链接动态库环境的对象
}

#elif  __linux__

#endif // WIN32