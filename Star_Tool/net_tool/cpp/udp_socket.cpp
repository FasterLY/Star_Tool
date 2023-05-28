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
		:ip_address(nullptr), close_flag(false), connect_flag(false)
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
			this->star_socket_handle = socket(AF_INET, SOCK_DGRAM, 0);
			break;
		case star::ip_type::ipv6:
			this->star_socket_handle = socket(AF_INET6, SOCK_DGRAM, 0);
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
	}
	udp_socket::udp_socket(udp_socket&& MoveSource) noexcept
		: close_flag(MoveSource.close_flag.load(std::memory_order_acquire)), star_socket_handle(std::move(MoveSource.star_socket_handle)),
		ip_address(std::move(MoveSource.ip_address)), connect_flag(MoveSource.connect_flag.load(std::memory_order_acquire))
	{
		MoveSource.star_socket_handle = STAR_INVALID_SOCKET;
		MoveSource.close_flag.store(true, std::memory_order_release);
		MoveSource.connect_flag.store(false, std::memory_order_release);
	}

	udp_socket::~udp_socket()
	{
		this->close();
	}

	star::udp_socket::udp_socket(std::string ip_or_domain, unsigned short port)
		:close_flag(false), connect_flag(true)
	{
#ifdef _WIN32
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!\n");
		}
#endif // _WIN32
		int ret;
		this->ip_address = std::make_shared<socket_addr_container>(ip_or_domain, port);
		switch (this->ip_address->ip_protocol)
		{
		case star::ip_type::ipv4:
			this->star_socket_handle = socket(AF_INET, SOCK_DGRAM, 0);
			break;
		case star::ip_type::ipv6:
			this->star_socket_handle = socket(AF_INET6, SOCK_DGRAM, 0);
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
		ret = connect(this->star_socket_handle, (star_sockaddr*)&(this->ip_address->ip_address), this->ip_address->addr_len);
		if (ret != 0) {
			this->close();
		}
	}
	int udp_socket::readfrom(socket_addr_container& address_buffer, char* buffer, int len, int offset)
	{
		int ret = -1;
		ret = ::recvfrom(this->star_socket_handle, buffer + offset, len, 0, (star_sockaddr*)&(address_buffer.ip_address), &(address_buffer.addr_len));
		return ret;
	}
	int udp_socket::writefor(socket_addr_container& destination, char* buffer, int len, int offset)
	{
		int ret = -1;
		ret = ::sendto(this->star_socket_handle, buffer + offset, len, 0, (star_sockaddr*)&(destination.ip_address), destination.addr_len);
		return ret;
	}
	int udp_socket::read(char* buffer, int len, int offset)
	{
		if (this->connect_flag.load(std::memory_order_acquire)) {
			int ret = ::recv(this->star_socket_handle, buffer + offset, len, 0);
			return ret;
		}
		else {
			return -1;
		}
	}
	int udp_socket::write(char* buffer, int len, int offset)
	{
		if (this->connect_flag.load(std::memory_order_acquire)) {
			int ret = ::send(this->star_socket_handle, buffer + offset, len, 0);
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
			closesocket(this->star_socket_handle);
#elif __linux__
			::close(this->star_socket_handle);
#endif
			this->star_socket_handle = STAR_INVALID_SOCKET;
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

	udp_socket_server::udp_socket_server(std::string ip_or_domain, unsigned short port)
		:close_flag(false)
	{
#ifdef _WIN32
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!\n");
		}
#endif // _WIN32
		int ret;
		this->ip_address = std::make_shared<socket_addr_container>(ip_or_domain, port);
		switch (this->ip_address->ip_protocol)
		{
		case star::ip_type::ipv4:
			this->star_socket_handle = socket(AF_INET, SOCK_DGRAM, 0);
			break;
		case star::ip_type::ipv6:
			this->star_socket_handle = socket(AF_INET6, SOCK_DGRAM, 0);
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
		ret = bind(this->star_socket_handle, (udp_socket::star_sockaddr*)&(this->ip_address->ip_address), this->ip_address->addr_len);
		if (ret != 0)
			throw net_exception("socket bind address fail!");
	}

	udp_socket_server::udp_socket_server(short port, star::ip_type IP_type)
	{
#ifdef _WIN32
		std::call_once(star::net_Initialize_flag, star::net_Initialize);
		if (!isInitialize.load()) {
			throw net_exception("windows net initialize fail!\n");
		}
#endif // _WIN32
		this->ip_address = std::make_shared<socket_addr_container>(port, IP_type);
		int ret;
		switch (this->ip_address->ip_protocol)
		{
		case star::ip_type::ipv4:
			this->star_socket_handle = socket(AF_INET, SOCK_DGRAM, 0);
			break;
		case star::ip_type::ipv6:
			this->star_socket_handle = socket(AF_INET6, SOCK_DGRAM, 0);
			break;
		default:
			throw net_exception("undefined ip type!");
			break;
		}
		ret = bind(this->star_socket_handle, (udp_socket::star_sockaddr*)&(this->ip_address->ip_address), this->ip_address->addr_len);
		if (ret != 0)
			throw net_exception("socket bind address fail!");
	}

	udp_socket_server::~udp_socket_server()
	{
		this->close();
	}

	unsigned short udp_socket_server::getPort()
	{
		if (this->ip_address == nullptr) {
			throw net_exception("socket address has been freed!");
		}
		switch (this->ip_address->ip_protocol)
		{
		case star::ip_type::ipv4:
			return ntohs(this->ip_address->ip_address.ipv4.sin_port);
			break;
		case star::ip_type::ipv6:
			return ntohs(this->ip_address->ip_address.ipv6.sin6_port);
			break;
		default:
			return 0;
			break;
		}
	}

	std::pair<int, socket_addr_container> udp_socket_server::read(char* buffer, int len, int offset)
	{
		socket_addr_container source{};
		int ret = ::recvfrom(this->star_socket_handle, buffer + offset, len, 0, (udp_socket::star_sockaddr*)&(source.ip_address), &(source.addr_len));
		return std::pair<int, socket_addr_container>(ret, std::move(source));
	}

	int udp_socket_server::write(socket_addr_container& destination, char* buffer, int len, int offset)
	{
		socket_addr_container source{};
		int ret = ::sendto(this->star_socket_handle, buffer + offset, len, 0, (udp_socket::star_sockaddr*)&(destination.ip_address), destination.addr_len);
		return ret;
	}

	void udp_socket_server::close()
	{
		if (!this->close_flag.load(std::memory_order_acquire)) {
			this->close_flag.store(true, std::memory_order_release);
#ifdef _WIN32
			::closesocket(this->star_socket_handle);
#elif __linux__
			::close(this->star_socket_handle);
#endif // _WIN32
			this->star_socket_handle = STAR_INVALID_SOCKET;
		}
	}

	bool udp_socket_server::isClose()
	{
		return this->close_flag.load(std::memory_order_acquire);
	}
}
