#ifndef NET_SHARED_CPP
#define NET_SHARED_CPP
#endif // !NET_SHARED_CPP

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
namespace star {
	std::atomic<bool> isInitialize(false);
	//封装动态链接库的初始化方法
	class Windows_net_Initialize {
	public:
		Windows_net_Initialize() {
			if (!isInitialize.load(std::memory_order_acquire)) {
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
	static std::shared_ptr<Windows_net_Initialize> windows_net_initialize_ptr;		//初始化链接动态库环境的对象
	std::once_flag net_Initialize_flag;
	void net_Initialize()
	{
		windows_net_initialize_ptr = std::make_shared<Windows_net_Initialize>();
	}
}

#elif  __linux__

#endif // WIN32