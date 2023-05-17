#include"../tcp_socket.h"
#ifdef WIN32
namespace star {
	tcp_socket::tcp_socket()
		:socket_client(INVALID_SOCKET), IP_type(star::ip_type::ipv4), close_flag(false)
	{
		memset(&ip_address, 0, sizeof(socket_addr));
	}

	tcp_socket::tcp_socket(tcp_socket&& MoveSource) noexcept
		: close_flag(MoveSource.close_flag.load()), socket_client(std::move(MoveSource.socket_client)),
		ip_address(std::move(MoveSource.ip_address)), address_len(MoveSource.address_len),
		IP_type(MoveSource.IP_type)
	{
		MoveSource.socket_client = INVALID_SOCKET;
		MoveSource.close_flag.store(false, std::memory_order_release);
	}

	tcp_socket::tcp_socket(std::string ip, unsigned short port, star::ip_type IP_type)
		:close_flag(false), IP_type(IP_type)
	{
		int ret;
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!\n");
		}
		switch (IP_type)
		{
		case star::ip_type::ipv4:
			ret = inet_pton(AF_INET, ip.c_str(), &(ip_address.ipv4.sin_addr));
			if (ret != 1) {
				throw net_exception("ip translation failed!\n");
			}
			ip_address.ipv4.sin_family = AF_INET;
			ip_address.ipv4.sin_port = htons(port);
			this->socket_client = socket(AF_INET, SOCK_STREAM, 0);
			this->address_len = sizeof(sockaddr_in);
			ret = connect(this->socket_client, (SOCKADDR*)&(this->ip_address.ipv4), address_len);
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
			this->socket_client = socket(AF_INET6, SOCK_STREAM, 0);
			this->address_len = sizeof(sockaddr_in6);
			ret = connect(this->socket_client, (SOCKADDR*)&(this->ip_address.ipv6), address_len);
			if (ret != 0) {
				this->close();
			}
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
	}

	int tcp_socket::read(char* buffer, int len, int offset) {
		if (!this->close_flag.load(std::memory_order::memory_order_acquire)) {
			int recv_len = ::recv(this->socket_client, buffer + offset, len, 0);
			return recv_len;
		}
		else {
			return -1;
		}
	}

	int tcp_socket::write(char* buffer, int len, int offset) {
		if (!this->close_flag.load(std::memory_order::memory_order_acquire)) {
			int send_len = ::send(this->socket_client, buffer + offset, len, 0);
			return send_len;
		}
		else {
			return -1;
		}
	}

	void tcp_socket::close() {
		this->close_flag.store(true, std::memory_order::memory_order_release);
		closesocket(this->socket_client);
	}

	void tcp_socket_server::Initialize(unsigned short port)
	{
		switch (this->IP_type)
		{
		case star::ip_type::ipv4:
			ip_address.ipv4.sin_family = AF_INET;
			ip_address.ipv4.sin_port = htons(port);
			this->socket_server = socket(AF_INET, SOCK_STREAM, 0);
			this->address_len = sizeof(sockaddr_in);
			bind(this->socket_server, (SOCKADDR*)&(this->ip_address.ipv4), address_len);
			break;
		case star::ip_type::ipv6:
			ip_address.ipv6.sin6_family = AF_INET6;
			ip_address.ipv6.sin6_port = htons(port);
			this->socket_server = socket(AF_INET6, SOCK_STREAM, 0);
			this->address_len = sizeof(sockaddr_in6);
			bind(this->socket_server, (SOCKADDR*)&(this->ip_address.ipv6), address_len);
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
		listen(this->socket_server, this->connect_num);
	}

	tcp_socket_server::tcp_socket_server(std::string ip, unsigned short port, int connect_num, star::ip_type IP_type)
		:close_flag(false), IP_type(IP_type), connect_num(connect_num)
	{
		int ret;
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!\n");
		}
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
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!\n");
		}
		switch (IP_type)
		{
		case star::ip_type::ipv4:
			ip_address.ipv4.sin_addr = in4addr_any;
			break;
		case star::ip_type::ipv6:
			ip_address.ipv6.sin6_addr = in6addr_any;
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
		this->Initialize(port);
	}

	tcp_socket tcp_socket_server::accept()
	{
		tcp_socket client_socket;
		switch (IP_type)
		{
		case star::ip_type::ipv4:
			client_socket.socket_client = ::accept(this->socket_server,
				(SOCKADDR*)&(client_socket.ip_address.ipv4),
				&(client_socket.address_len));
			client_socket.IP_type = star::ip_type::ipv4;
			break;
		case star::ip_type::ipv6:
			client_socket.socket_client = ::accept(this->socket_server,
				(SOCKADDR*)&(client_socket.ip_address.ipv6),
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
		this->close_flag.store(true, std::memory_order_release);
		::closesocket(this->socket_server);
	}

	bool tcp_socket_server::isClose()
	{
		return this->close_flag.load(std::memory_order_acquire);
	}
}
#elif __linux__
namespace star {
	tcp_socket::tcp_socket()
		:socket_client(-1), IP_type(star::ip_type::ipv4), close_flag(false)
	{
		memset(&ip_address, 0, sizeof(socket_addr));
	}

	tcp_socket::tcp_socket(tcp_socket&& MoveSource) noexcept
		: close_flag(MoveSource.close_flag.load()), socket_client(std::move(MoveSource.socket_client)),
		ip_address(std::move(MoveSource.ip_address)), address_len(MoveSource.address_len),
		IP_type(MoveSource.IP_type)
	{
		MoveSource.socket_client = -1;
		MoveSource.close_flag.store(false, std::memory_order_release);
	}

	tcp_socket::tcp_socket(std::string ip, unsigned short port, star::ip_type IP_type)
		:IP_type(IP_type), close_flag(false)
	{
		int ret;
		switch (IP_type)
		{
		case star::ip_type::ipv4:
			ret = inet_pton(AF_INET, ip.c_str(), &(ip_address.ipv4.sin_addr));
			if (ret != 1) {
				throw net_exception("ip translation failed!\n");
			}
			ip_address.ipv4.sin_family = AF_INET;
			ip_address.ipv4.sin_port = htons(port);
			this->socket_client = socket(AF_INET, SOCK_STREAM, 0);
			this->address_len = sizeof(sockaddr_in);
			ret = connect(this->socket_client, (sockaddr*)&(this->ip_address.ipv4), address_len);
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
			this->socket_client = socket(AF_INET6, SOCK_STREAM, 0);
			this->address_len = sizeof(sockaddr_in6);
			ret = connect(this->socket_client, (sockaddr*)&(this->ip_address.ipv6), address_len);
			if (ret != 0) {
				this->close();
			}
			break;
		default:
			break;
		}
	}

	int tcp_socket::read(char* buffer, int len, int offset)
	{
		if (!this->close_flag.load(std::memory_order::memory_order_acquire)) {
			int recv_len = ::recv(this->socket_client, buffer + offset, len, 0);
			return recv_len;
		}
		else {
			return -1;
		}
	}

	int tcp_socket::write(char* buffer, int len, int offset)
	{
		if (!this->close_flag.load(std::memory_order::memory_order_acquire)) {
			int send_len = ::send(this->socket_client, buffer + offset, len, 0);
			return send_len;
		}
		else {
			return -1;
		}
	}

	void tcp_socket::close()
	{
		this->close_flag.store(true, std::memory_order::memory_order_release);
		::close(this->socket_client);
	}

	void tcp_socket_server::Initialize(unsigned short port)
	{
		switch (this->IP_type)
		{
		case star::ip_type::ipv4:
			ip_address.ipv4.sin_family = AF_INET;
			ip_address.ipv4.sin_port = htons(port);
			this->socket_server = socket(AF_INET, SOCK_STREAM, 0);
			this->address_len = sizeof(sockaddr_in);
			bind(this->socket_server, (sockaddr*)&(this->ip_address.ipv4), address_len);
			break;
		case star::ip_type::ipv6:
			ip_address.ipv6.sin6_family = AF_INET6;
			ip_address.ipv6.sin6_port = htons(port);
			this->socket_server = socket(AF_INET6, SOCK_STREAM, 0);
			this->address_len = sizeof(sockaddr_in6);
			bind(this->socket_server, (sockaddr*)&(this->ip_address.ipv6), address_len);
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
		listen(this->socket_server, this->connect_num);
	}

	tcp_socket_server::tcp_socket_server(std::string ip, unsigned short port, int connect_num, star::ip_type IP_type)
		:close_flag(false), IP_type(IP_type), connect_num(connect_num)
	{
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

	tcp_socket_server::tcp_socket_server(short port, int connect_num, star::ip_type IP_type)
		:close_flag(false), IP_type(IP_type), connect_num(connect_num)
	{
		switch (IP_type)
		{
		case star::ip_type::ipv4:
			ip_address.ipv4.sin_addr.s_addr = INADDR_ANY;
			break;
		case star::ip_type::ipv6:
			ip_address.ipv6.sin6_addr = IN6ADDR_ANY_INIT;
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
		this->Initialize(port);
	}

	tcp_socket tcp_socket_server::accept()
	{
		tcp_socket client_socket;
		switch (IP_type)
		{
		case star::ip_type::ipv4:
			client_socket.socket_client = ::accept(this->socket_server,
				(sockaddr*)&(client_socket.ip_address.ipv4),
				&(client_socket.address_len));
			client_socket.IP_type = star::ip_type::ipv4;
			break;
		case star::ip_type::ipv6:
			client_socket.socket_client = ::accept(this->socket_server,
				(sockaddr*)&(client_socket.ip_address.ipv6),
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
		this->close_flag.store(true, std::memory_order_release);
		::close(this->socket_server);
	}

	bool tcp_socket_server::isClose()
	{
		return this->close_flag.load(std::memory_order_acquire);
	}
}
#endif // WIN32

