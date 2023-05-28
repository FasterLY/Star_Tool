#include"../net_shared.h"
#ifdef _WIN32
#define STAR_INVALID_SOCKET INVALID_SOCKET
#define STAR_IN4ADDR_ANY in4addr_any
#define STAR_IN6ADDR_ANY in6addr_any
#elif __linux__
#define STAR_INVALID_SOCKET -1
#define STAR_IN4ADDR_ANY htonl(INADDR_ANY)
#define STAR_IN6ADDR_ANY IN6ADDR_ANY_INIT
#endif // WIN32
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

namespace star {
	socket_addr_container::socket_addr_container(unsigned short port, ip_type ip_protocol)
		:ip_protocol(ip_protocol)
	{
#ifdef _WIN32
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!\n");
		}
#endif // _WIN32
		memset(this, 0, sizeof(socket_addr_container));
		switch (ip_protocol)
		{
		case star::ip_type::ipv4:
			this->addr_len = sizeof(sockaddr_in);
			this->ip_protocol = star::ip_type::ipv4;
#ifdef _WIN32
			this->ip_address.ipv4.sin_addr = STAR_IN4ADDR_ANY;
#elif __linux__
			this->ip_address.ipv4.sin_addr.s_addr = STAR_IN4ADDR_ANY;
#endif // _WIN32
			this->ip_address.ipv4.sin_family = AF_INET;
			this->ip_address.ipv4.sin_port = htons(port);
			break;
		case star::ip_type::ipv6:
			this->addr_len = sizeof(sockaddr_in6);
			this->ip_protocol = star::ip_type::ipv4;
			this->ip_address.ipv6.sin6_addr = STAR_IN6ADDR_ANY;
			this->ip_address.ipv6.sin6_family = AF_INET6;
			this->ip_address.ipv6.sin6_port = htons(port);
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
	}

	socket_addr_container::socket_addr_container()
	{
		memset(this, 0, sizeof(socket_addr_container));
	}

	socket_addr_container::socket_addr_container(std::string domain)
	{
#ifdef _WIN32
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!\n");
		}
#endif // _WIN32
		int ret;
		memset(this, 0, sizeof(socket_addr_container));
		addrinfo* result;
		ret = getaddrinfo(domain.c_str(), nullptr, nullptr, &result);
		if (ret == 0)
		{
			switch (result->ai_family)
			{
			case AF_INET:
				this->addr_len = result->ai_addrlen;
				this->ip_protocol = star::ip_type::ipv4;
				this->ip_address.ipv4 = *((struct sockaddr_in*)result->ai_addr);
				break;
			case AF_INET6:
				this->addr_len = result->ai_addrlen;
				this->ip_protocol = star::ip_type::ipv4;
				this->ip_address.ipv6 = *((struct sockaddr_in6*)result->ai_addr);
				break;
			default:
				freeaddrinfo(result);
				throw net_exception("IP or DNS analysis failed!");
				break;
			}
			freeaddrinfo(result);
		}
		else
		{
			throw net_exception("IP or DNS analysis failed!");
		}
	}

	socket_addr_container::socket_addr_container(std::string ip, unsigned short port)
	{
#ifdef _WIN32
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!\n");
		}
#endif // _WIN32
		int ret;
		memset(this, 0, sizeof(socket_addr_container));
		addrinfo* result;
		ret = getaddrinfo(ip.c_str(), nullptr, nullptr, &result);
		if (ret == 0)
		{
			switch (result->ai_family)
			{
			case AF_INET:
				this->addr_len = result->ai_addrlen;
				this->ip_protocol = star::ip_type::ipv4;
				this->ip_address.ipv4 = *((struct sockaddr_in*)result->ai_addr);
				this->ip_address.ipv4.sin_family = AF_INET;
				this->ip_address.ipv4.sin_port = htons(port);
				break;
			case AF_INET6:
				this->addr_len = result->ai_addrlen;
				this->ip_protocol = star::ip_type::ipv4;
				this->ip_address.ipv6 = *((struct sockaddr_in6*)result->ai_addr);
				this->ip_address.ipv6.sin6_family = AF_INET6;
				this->ip_address.ipv6.sin6_port = htons(port);
				break;
			default:
				freeaddrinfo(result);
				throw net_exception("IP or DNS analysis failed!");
				break;
			}
			freeaddrinfo(result);
		}
		else
		{
			throw net_exception("IP or DNS analysis failed!");
		}
	}

	socket_addr_container::socket_addr_container(socket_addr_container&& MoveSource) noexcept
		:ip_address(MoveSource.ip_address), addr_len(MoveSource.addr_len), ip_protocol(MoveSource.ip_protocol)
	{
		memset(&MoveSource, 0, sizeof(MoveSource));
	}
}