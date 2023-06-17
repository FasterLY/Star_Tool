#include"../tcp_socket.h"
#ifdef _WIN32							//Windows头文件
#pragma comment(lib,"ws2_32.lib")		//链接动态库
#ifndef _WINSOCKAPI_					//防止重复载入头文件
#include <Winsock2.h>  
#include<WS2tcpip.h>
#endif
#elif __linux__
#include<sys/socket.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<netdb.h>
#endif
#include<atomic>


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
#ifdef _WIN32
	extern std::atomic<bool> isInitialize; 
	extern std::once_flag net_Initialize_flag;
	extern void net_Initialize();
#endif
	tcp_socket::tcp_socket()
		:star_socket_handle(STAR_INVALID_SOCKET), close_flag(false), block_flag(true), ip_address(std::make_shared<socket_addr_container>())
	{}

	tcp_socket::tcp_socket(tcp_socket&& MoveSource) noexcept
		: close_flag(MoveSource.close_flag.load()), star_socket_handle(std::move(MoveSource.star_socket_handle)),
		ip_address(std::move(MoveSource.ip_address)), block_flag(MoveSource.block_flag.load())
	{
		MoveSource.star_socket_handle = STAR_INVALID_SOCKET;
		MoveSource.close_flag.store(true, std::memory_order_release);
	}

	tcp_socket::tcp_socket(std::string ip_or_domain, unsigned short port)
		:close_flag(false), block_flag(true)
	{
#ifdef _WIN32
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!");
		}
#endif // _WIN32
		this->ip_address = std::make_shared<socket_addr_container>(ip_or_domain, port);
		int ret;
		switch (this->ip_address->ip_protocol)
		{
		case star::ip_type::ipv4:
			this->star_socket_handle = socket(AF_INET, SOCK_STREAM, 0);
			break;
		case star::ip_type::ipv6:
			this->star_socket_handle = socket(AF_INET6, SOCK_STREAM, 0);
			break;
		default:
			throw net_exception("undefined ip type!");
			break;
		}
		ret = connect(this->star_socket_handle, (star_sockaddr*)&(this->ip_address->ip_address), this->ip_address->addr_len);
		if (ret != 0) {
			this->close();
		}
	}

	tcp_socket::tcp_socket(std::string domain_name)
	{
#ifdef _WIN32
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!");
		}
#endif // _WIN32
		this->ip_address = std::make_shared<socket_addr_container>(domain_name);
		int ret;
		switch (this->ip_address->ip_protocol)
		{
		case star::ip_type::ipv4:
			this->star_socket_handle = socket(AF_INET, SOCK_STREAM, 0);
			break;
		case star::ip_type::ipv6:
			this->star_socket_handle = socket(AF_INET6, SOCK_STREAM, 0);
			break;
		default:
			throw net_exception("undefined ip type!");
			break;
		}
		ret = connect(this->star_socket_handle, (star_sockaddr*)&(this->ip_address->ip_address), this->ip_address->addr_len);
		if (ret != 0) {
			this->close();
		}
	}

	tcp_socket::~tcp_socket()
	{
		this->close();
	}

	int tcp_socket::read(char* buffer, int len, int offset) {
		if (!this->close_flag.load(std::memory_order::memory_order_acquire)) {
			int recv_len = ::recv(this->star_socket_handle, buffer + offset, len, 0);
			return recv_len;
		}
		else {
			return -1;
		}
	}

	int tcp_socket::write(char* buffer, int len, int offset) {
		if (!this->close_flag.load(std::memory_order::memory_order_acquire)) {
			int send_len = ::send(this->star_socket_handle, buffer + offset, len, 0);
			return send_len;
		}
		else {
			return -1;
		}
	}

	bool tcp_socket::setblock(bool block)
	{
		if (this->block_flag.load() && !block) {		//设置非阻塞
#ifdef WIN32
			u_long argp = 1;
			if (ioctlsocket(this->star_socket_handle, FIONBIO, &argp) != 0) {
				return false;
			}
#elif __linux
			int flags = fcntl(this->star_socket_handle, F_GETFL, 0);
			if (flags < 0) {
				return false;
			}
			flags |= O_NONBLOCK;
			if (fcntl(this->star_socket_handle, F_SETFL, flags) < 0) {
				return false;
			}
#endif // WIN32
		}
		else if (!this->block_flag.load() && block) {	//设置阻塞
#ifdef WIN32
			u_long argp = 0;
			if (ioctlsocket(this->star_socket_handle, FIONBIO, &argp) != 0) {
				return false;
			}
#elif __linux
			int flags = fcntl(this->star_socket_handle, F_GETFL, 0);
			if (flags < 0) {
				return false;
			}
			flags &= ~O_NONBLOCK;
			if (fcntl(this->star_socket_handle, F_SETFL, flags) < 0) {
				return false;
			}
#endif // WIN32
		}
		return true;
	}

	bool tcp_socket::isblock()
	{
		return this->block_flag.load(std::memory_order_acquire);
	}

	int tcp_socket::availavle()
	{
		unsigned long availavle = 0;
		if (
#ifdef _WIN32
			::ioctlsocket(this->star_socket_handle, FIONREAD, &availavle)
#elif __linux__
			::ioctl(this->star_socket_handle, FIONREAD, &availavle)
#endif // _WIN32
			) {
			return availavle;
		}
		else {
			return -1;
		}
	}

	void tcp_socket::close() {
		if (!close_flag.load(std::memory_order_acquire)) {
			this->close_flag.store(true, std::memory_order::memory_order_release);
#ifdef _WIN32
			closesocket(this->star_socket_handle);
#elif __linux__
			::close(this->star_socket_handle);
#endif
			this->star_socket_handle = STAR_INVALID_SOCKET;
		}
	}

	bool tcp_socket::isClose()
	{
		return this->close_flag.load(std::memory_order_acquire);
	}

	void tcp_socket::freeAddr()
	{
		this->ip_address = nullptr;
	}

	std::shared_ptr<socket_addr_container> tcp_socket::getAddr()
	{
		return this->ip_address;
	}

	tcp_socket_server::tcp_socket_server(std::string ip_or_domain, unsigned short port, int connect_num)
		:close_flag(false), connect_num(connect_num)
	{
#ifdef _WIN32
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!");
		}
#endif // _WIN32
		this->ip_address = std::make_shared<socket_addr_container>(ip_or_domain, port);
		int ret;
		switch (this->ip_address->ip_protocol)
		{
		case star::ip_type::ipv4:
			this->star_socket_handle = socket(AF_INET, SOCK_STREAM, 0);
			break;
		case star::ip_type::ipv6:
			this->star_socket_handle = socket(AF_INET6, SOCK_STREAM, 0);
			break;
		default:
			throw net_exception("undefined ip type!");
			break;
		}
		ret = bind(this->star_socket_handle, (tcp_socket::star_sockaddr*)&(this->ip_address->ip_address), this->ip_address->addr_len);
		if (ret != 0)
			throw net_exception("socket bind address fail!");
		ret = listen(this->star_socket_handle, this->connect_num);
		if (ret != 0)
			throw net_exception("socket set listen state fail!");
	}

	tcp_socket_server::tcp_socket_server(unsigned short port, star::ip_type IP_type, int connect_num)
		:close_flag(false), connect_num(connect_num)
	{
#ifdef _WIN32
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!");
		}
#endif // _WIN32
		this->ip_address = std::make_shared<socket_addr_container>(port, IP_type);
		int ret;
		switch (this->ip_address->ip_protocol)
		{
		case star::ip_type::ipv4:
			this->star_socket_handle = socket(AF_INET, SOCK_STREAM, 0);
			break;
		case star::ip_type::ipv6:
			this->star_socket_handle = socket(AF_INET6, SOCK_STREAM, 0);
			break;
		default:
			throw net_exception("undefined ip type!");
			break;
		}
		ret = bind(this->star_socket_handle, (tcp_socket::star_sockaddr*)&(this->ip_address->ip_address), this->ip_address->addr_len);
		if (ret != 0)
			throw net_exception("socket bind address fail!");
		ret = listen(this->star_socket_handle, this->connect_num);
		if (ret != 0)
			throw net_exception("socket set listen state fail!");
	}

	tcp_socket_server::~tcp_socket_server()
	{
		this->close();
	}

	tcp_socket tcp_socket_server::accept()
	{
		tcp_socket client_socket{};
		ip_type socket_type;
		if (this->ip_address != nullptr)
		{
			socket_type = this->ip_address->ip_protocol;
		}
		else //需要从内核中判断协议族类型
		{
			struct sockaddr_storage addr;
			socklen_t addrlen = sizeof(addr);
			int ret = getsockname(this->star_socket_handle, (struct sockaddr*)&addr, &addrlen);
			if (addr.ss_family == AF_INET)
				socket_type = star::ip_type::ipv4;
			else if (addr.ss_family == AF_INET6)
				socket_type = star::ip_type::ipv4;
			else
				throw net_exception("ip type get fail!");
		}
		switch (socket_type)
		{
		case star::ip_type::ipv4:
			client_socket.ip_address->addr_len = sizeof(sockaddr_in);
			break;
		case star::ip_type::ipv6:
			client_socket.ip_address->addr_len = sizeof(sockaddr_in6);
			break;
		default:
			break;
		}
		client_socket.star_socket_handle = ::accept(this->star_socket_handle,
			(tcp_socket::star_sockaddr*)&(client_socket.ip_address->ip_address),
			&(client_socket.ip_address->addr_len));
		return client_socket;
	}

	unsigned short tcp_socket_server::getPort()
	{
		if (this->ip_address == nullptr) {
			struct sockaddr_storage addr;
			socklen_t addrlen = sizeof(addr);
			int ret = getsockname(this->star_socket_handle, (struct sockaddr*)&addr, &addrlen);
			if (addr.ss_family == AF_INET) {
				struct sockaddr_in* sin_ptr = (struct sockaddr_in*)&addr;
				return ntohs(sin_ptr->sin_port);
			}
			else if (addr.ss_family == AF_INET6) {
				struct sockaddr_in6* sin6_ptr = (struct sockaddr_in6*)&addr;
				return ntohs(sin6_ptr->sin6_port);
			}
			else {
				throw net_exception("socket address has been freed ! option in system has been deleted !");
			}
		}
		else {
			return this->ip_address->getPort();
		}
	}

	void tcp_socket_server::close()
	{
		if (!close_flag.load(std::memory_order_acquire)) {
			this->close_flag.store(true, std::memory_order::memory_order_release);
#ifdef _WIN32
			closesocket(this->star_socket_handle);
#elif __linux__
			::close(this->star_socket_handle);
#endif
			this->star_socket_handle = STAR_INVALID_SOCKET;
		}
	}

	bool tcp_socket_server::isClose()
	{
		return this->close_flag.load(std::memory_order_acquire);
	}

	void tcp_socket_server::freeAddr()
	{
		this->ip_address = nullptr;
	}

	std::shared_ptr<socket_addr_container> tcp_socket_server::getAddr()
	{
		return this->ip_address;
	}

	std::string tcp_socket_server::getAddr_Str()
	{
		if (this->ip_address == nullptr) {
			struct sockaddr_storage addr;
			socklen_t addrlen = sizeof(addr);
			int ret = getsockname(this->star_socket_handle, (struct sockaddr*)&addr, &addrlen);
			if (addr.ss_family == AF_INET) {
				struct sockaddr_in* sin_ptr = (struct sockaddr_in*)&addr;
				char ip[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &(sin_ptr->sin_addr), ip, INET_ADDRSTRLEN);
				return ip;
			}
			else if (addr.ss_family == AF_INET6) {
				struct sockaddr_in6* sin6_ptr = (struct sockaddr_in6*)&addr;
				char ip[INET6_ADDRSTRLEN];
				inet_ntop(AF_INET, &(sin6_ptr->sin6_addr), ip, INET6_ADDRSTRLEN);
				return ip;
			}
			else {
				throw net_exception("socket address has been freed ! option in system has been deleted !");
			}
		}
		else {
			return this->ip_address->getAddr_Str();
		}
	}
}