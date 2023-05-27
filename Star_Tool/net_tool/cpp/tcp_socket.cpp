#include"../tcp_socket.h"
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
		:star_socket_handle(STAR_INVALID_SOCKET), IP_type(star::ip_type::ipv4), close_flag(false), block_flag(true)
	{
		memset(&ip_address, 0, sizeof(socket_addr));
	}

	tcp_socket::tcp_socket(tcp_socket&& MoveSource) noexcept
		: close_flag(MoveSource.close_flag.load()), star_socket_handle(std::move(MoveSource.star_socket_handle)),
		ip_address(std::move(MoveSource.ip_address)), address_len(MoveSource.address_len),
		IP_type(MoveSource.IP_type), block_flag(MoveSource.block_flag.load())
	{
		MoveSource.star_socket_handle = STAR_INVALID_SOCKET;
		MoveSource.close_flag.store(true, std::memory_order_release);
	}

	tcp_socket::tcp_socket(std::string ip, unsigned short port, star::ip_type IP_type)
		:close_flag(false), IP_type(IP_type), block_flag(true)
	{
#ifdef _WIN32
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!\n");
		}
#endif // _WIN32
		int ret;
		memset(&ip_address, 0, sizeof(socket_addr));
		switch (IP_type)
		{
		case star::ip_type::ipv4:
			ret = inet_pton(AF_INET, ip.c_str(), &(ip_address.ipv4.sin_addr));
			if (ret != 1) {
				throw net_exception("ip translation failed!\n");
			}
			ip_address.ipv4.sin_family = AF_INET;
			ip_address.ipv4.sin_port = htons(port);
			this->star_socket_handle = socket(AF_INET, SOCK_STREAM, 0);
			this->address_len = sizeof(sockaddr_in);
			ret = connect(this->star_socket_handle, (star_sockaddr*)&(this->ip_address.ipv4), address_len);
			if (ret != 0) {
				this->close();
			}
			break;
		case star::ip_type::ipv6:
			ret = inet_pton(AF_INET6, ip.c_str(), &(ip_address.ipv6.sin6_addr));
			if (ret != 1) {
				throw net_exception("ip translation failed!\n");
			}
			ip_address.ipv6.sin6_family = AF_INET6;
			ip_address.ipv6.sin6_port = htons(port);
			this->star_socket_handle = socket(AF_INET6, SOCK_STREAM, 0);
			this->address_len = sizeof(sockaddr_in6);
			ret = connect(this->star_socket_handle, (star_sockaddr*)&(this->ip_address.ipv6), address_len);
			if (ret != 0) {
				this->close();
			}
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
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
		if (this->block_flag.load() && !block) {		//���÷�����
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
		else if (!this->block_flag.load() && block) {	//��������
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

	void tcp_socket_server::Initialize(unsigned short port)
	{
		switch (this->IP_type)
		{
		case star::ip_type::ipv4:
			ip_address.ipv4.sin_family = AF_INET;
			ip_address.ipv4.sin_port = htons(port);
			this->star_socket_handle = socket(AF_INET, SOCK_STREAM, 0);
			this->address_len = sizeof(sockaddr_in);
			bind(this->star_socket_handle, (tcp_socket::star_sockaddr*)&(this->ip_address.ipv4), address_len);
			break;
		case star::ip_type::ipv6:
			ip_address.ipv6.sin6_family = AF_INET6;
			ip_address.ipv6.sin6_port = htons(port);
			this->star_socket_handle = socket(AF_INET6, SOCK_STREAM, 0);
			this->address_len = sizeof(sockaddr_in6);
			bind(this->star_socket_handle, (tcp_socket::star_sockaddr*)&(this->ip_address.ipv6), address_len);
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
		listen(this->star_socket_handle, this->connect_num);
	}

	tcp_socket_server::tcp_socket_server(std::string ip, unsigned short port, int connect_num, star::ip_type IP_type)
		:close_flag(false), IP_type(IP_type), connect_num(connect_num)
	{
#ifdef _WIN32
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!\n");
		}
#endif // _WIN32
		int ret;
		switch (IP_type)
		{
		case star::ip_type::ipv4:
			ret = inet_pton(AF_INET, ip.c_str(), &(ip_address.ipv4.sin_addr));
			if (ret != 1) {
				throw net_exception("ip translation failed!\n");
			}
			break;
		case star::ip_type::ipv6:
			ret = inet_pton(AF_INET6, ip.c_str(), &(ip_address.ipv6.sin6_addr));
			if (ret != 1) {
				throw net_exception("ip translation failed!\n");
			}
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
		this->Initialize(port);
	}

	tcp_socket_server::tcp_socket_server(unsigned short port, int connect_num, star::ip_type IP_type)
		:close_flag(false), IP_type(IP_type), connect_num(connect_num)
	{
#ifdef _WIN32
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!\n");
		}
#endif // _WIN32
		switch (IP_type)
		{
		case star::ip_type::ipv4:
#ifdef _WIN32
			ip_address.ipv4.sin_addr = STAR_IN4ADDR_ANY;
#elif __linux__
			ip_address.ipv4.sin_addr.s_addr = STAR_IN4ADDR_ANY;
#endif // _WIN32
			break;
		case star::ip_type::ipv6:
			ip_address.ipv6.sin6_addr = STAR_IN6ADDR_ANY;
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
		this->Initialize(port);
	}

	tcp_socket_server::~tcp_socket_server()
	{
		this->close();
	}

	tcp_socket tcp_socket_server::accept()
	{
		tcp_socket client_socket;
		switch (IP_type)
		{
		case star::ip_type::ipv4:
			client_socket.star_socket_handle = ::accept(this->star_socket_handle,
				(tcp_socket::star_sockaddr*)&(client_socket.ip_address.ipv4),
				&(client_socket.address_len));
			client_socket.IP_type = star::ip_type::ipv4;
			break;
		case star::ip_type::ipv6:
			client_socket.star_socket_handle = ::accept(this->star_socket_handle,
				(tcp_socket::star_sockaddr*)&(client_socket.ip_address.ipv6),
				&(client_socket.address_len));
			client_socket.IP_type = star::ip_type::ipv6;
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
		return client_socket;
	}

	unsigned short tcp_socket_server::getPort()
	{
		switch (this->IP_type)
		{
		case star::ip_type::ipv4:
			return ntohs(this->ip_address.ipv4.sin_port);
			break;
		case star::ip_type::ipv6:
			return ntohs(this->ip_address.ipv6.sin6_port);
			break;
		default:
			return 0;
			break;
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
}