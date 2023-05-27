#include "../udp_socket.h"
#ifdef _WIN32
#define STAR_INVALID_SOCKET INVALID_SOCKET
#define STAR_IN4ADDR_ANY in4addr_any
#define STAR_IN6ADDR_ANY in6addr_any
#elif __linux__
#define STAR_INVALID_SOCKET -1
#define STAR_IN4ADDR_ANY INADDR_ANY
#define STAR_IN6ADDR_ANY IN6ADDR_ANY_INIT
#endif // WIN32
namespace star {
#ifdef _WIN32
	extern std::atomic<bool> isInitialize;
	extern std::once_flag net_Initialize_flag;
	extern void net_Initialize();
#endif
	udp_socket::udp_socket(star::ip_type IP_type)
		:ip_address(nullptr), close_flag(false), IP_type(IP_type), address_len(0), connect_flag(false)
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
			this->socket_client = socket(AF_INET, SOCK_DGRAM, 0);
			break;
		case star::ip_type::ipv6:
			this->socket_client = socket(AF_INET6, SOCK_DGRAM, 0);
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
	}
	udp_socket::udp_socket(udp_socket&& MoveSource) noexcept
		: close_flag(MoveSource.close_flag.load(std::memory_order_acquire)), socket_client(std::move(MoveSource.socket_client)),
		ip_address(std::move(MoveSource.ip_address)), address_len(MoveSource.address_len),
		IP_type(MoveSource.IP_type), connect_flag(MoveSource.connect_flag.load(std::memory_order_acquire))
	{
		MoveSource.socket_client = STAR_INVALID_SOCKET;
		MoveSource.close_flag.store(true, std::memory_order_release);
		MoveSource.connect_flag.store(false, std::memory_order_release);
	}

	star::udp_socket::udp_socket(std::string ip, unsigned short port, star::ip_type IP_type)
		:close_flag(false), IP_type(IP_type), connect_flag(true)
	{
#ifdef _WIN32
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!\n");
		}
#endif // _WIN32
		int ret;
		this->ip_address = std::make_unique<socket_addr>();
		switch (IP_type)
		{
		case star::ip_type::ipv4:
			ret = inet_pton(AF_INET, ip.c_str(), &(ip_address->ipv4.sin_addr));
			if (ret != 1) {
				throw net_exception("ip translation failed!\n");
			}
			ip_address->ipv4.sin_family = AF_INET;
			ip_address->ipv4.sin_port = htons(port);
			this->socket_client = socket(AF_INET, SOCK_DGRAM, 0);
			this->address_len = sizeof(sockaddr_in);
			ret = connect(this->socket_client, (star_sockaddr*)&(this->ip_address->ipv4), address_len);
			if (ret != 0) {
				this->close();
			}
			break;
		case star::ip_type::ipv6:
			ret = inet_pton(AF_INET6, ip.c_str(), &(ip_address->ipv6.sin6_addr));
			if (ret != 1) {
				throw net_exception("ip translation failed!\n");
			}
			ip_address->ipv6.sin6_family = AF_INET6;
			ip_address->ipv6.sin6_port = htons(port);
			this->socket_client = socket(AF_INET6, SOCK_DGRAM, 0);
			this->address_len = sizeof(sockaddr_in6);
			ret = connect(this->socket_client, (star_sockaddr*)&(this->ip_address->ipv6), address_len);
			if (ret != 0) {
				this->close();
			}
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
	}
	int udp_socket::readfrom(socket_addr_container& address_buffer, char* buffer, int len, int offset)
	{
		int ret = -1;
		ret = ::recvfrom(this->socket_client, buffer + offset, len, 0, (star_sockaddr*)&(address_buffer.ip_address), &(address_buffer.addr_len));
		return ret;
	}
	int udp_socket::writefor(socket_addr_container& destination, char* buffer, int len, int offset)
	{
		int ret = -1;
		ret = ::sendto(this->socket_client, buffer + offset, len, 0, (star_sockaddr*)&(destination.ip_address), destination.addr_len);
		return ret;
	}
	int udp_socket::read(char* buffer, int len, int offset)
	{
		if (this->connect_flag.load(std::memory_order_acquire)) {
			int ret = ::recv(this->socket_client, buffer + offset, len, 0);
			return ret;
		}
		else {
			return -1;
		}
	}
	int udp_socket::write(char* buffer, int len, int offset)
	{
		if (this->connect_flag.load(std::memory_order_acquire)) {
			int ret = ::send(this->socket_client, buffer + offset, len, 0);
			return ret;
		}
		else {
			return -1;
		}
	}
	void udp_socket::close()
	{
		if (!close_flag.load(std::memory_order_acquire)) {
			this->close_flag.store(true, std::memory_order::memory_order_release);
#ifdef _WIN32
			closesocket(this->socket_client);
#elif __linux__
			::close(this->socket_client);
#endif
			this->socket_client = STAR_INVALID_SOCKET;
		}
	}
	bool udp_socket::isClose()
	{
		return this->close_flag.load(std::memory_order_acquire);
	}
	bool udp_socket::isConnected()
	{
		return this->connect_flag.load(std::memory_order_acquire);
	}

	void udp_socket_server::Initialize(unsigned short port)
	{
		switch (this->IP_type)
		{
		case star::ip_type::ipv4:
			ip_address.ipv4.sin_family = AF_INET;
			ip_address.ipv4.sin_port = htons(port);
			this->socket_server = socket(AF_INET, SOCK_DGRAM, 0);
			this->address_len = sizeof(sockaddr_in);
			bind(this->socket_server, (udp_socket::star_sockaddr*)&(this->ip_address.ipv4), address_len);
			break;
		case star::ip_type::ipv6:
			ip_address.ipv6.sin6_family = AF_INET6;
			ip_address.ipv6.sin6_port = htons(port);
			this->socket_server = socket(AF_INET6, SOCK_DGRAM, 0);
			this->address_len = sizeof(sockaddr_in6);
			bind(this->socket_server, (udp_socket::star_sockaddr*)&(this->ip_address.ipv6), address_len);
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
	}
	udp_socket_server::udp_socket_server(std::string ip, unsigned short port, int connect_num, star::ip_type IP_type)
		:close_flag(false), IP_type(IP_type)
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
	udp_socket_server::udp_socket_server(short port, int connect_num, star::ip_type IP_type)
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
			ip_address.ipv4.sin_addr = STAR_IN4ADDR_ANY;
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
	unsigned short udp_socket_server::getPort()
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
	std::pair<int, socket_addr_container> udp_socket_server::read(char* buffer, int len, int offset)
	{
		socket_addr_container source{};
		int ret = ::recvfrom(this->socket_server, buffer + offset, len, 0, (udp_socket::star_sockaddr*)&(source.ip_address), &(source.addr_len));
		return std::pair<int, socket_addr_container>(ret, std::move(source));
	}
	int udp_socket_server::write(socket_addr_container& destination, char* buffer, int len, int offset)
	{
		socket_addr_container source{};
		int ret = ::sendto(this->socket_server, buffer + offset, len, 0, (udp_socket::star_sockaddr*)&(destination.ip_address), destination.addr_len);
		return ret;
	}
	void udp_socket_server::close()
	{
		this->close_flag.store(true, std::memory_order_release);
		::closesocket(this->socket_server);
	}
	bool udp_socket_server::isClose()
	{
		return this->close_flag.load(std::memory_order_acquire);
	}

	socket_addr_container::socket_addr_container()
		:addr_len(0)
	{
		memset(this, 0, sizeof(socket_addr_container));
	}

	socket_addr_container::socket_addr_container(std::string ip, unsigned short port, star::ip_type IP_type)
	{
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
			this->addr_len = sizeof(sockaddr_in);
			break;
		case star::ip_type::ipv6:
			ret = inet_pton(AF_INET6, ip.c_str(), &(ip_address.ipv6.sin6_addr));
			if (ret != 1) {
				throw net_exception("ip translation failed!\n");
			}
			ip_address.ipv6.sin6_family = AF_INET6;
			ip_address.ipv6.sin6_port = htons(port);
			this->addr_len = sizeof(sockaddr_in6);
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
	}

	socket_addr_container::socket_addr_container(socket_addr_container&& MoveSource) noexcept
		:ip_address(MoveSource.ip_address), addr_len(MoveSource.addr_len)
	{
		memset(&MoveSource.ip_address, 0, sizeof(socket_addr));
		MoveSource.addr_len = 0;
	}

		
}
