#include "../udp_socket.h"
#ifdef _WIN32
namespace star {
	udp_socket::udp_socket(udp_socket&& MoveSource) noexcept
		: close_flag(MoveSource.close_flag.load()), socket_client(std::move(MoveSource.socket_client)),
		ip_address(std::move(MoveSource.ip_address)), address_len(MoveSource.address_len),
		IP_type(MoveSource.IP_type)
	{
		MoveSource.socket_client = INVALID_SOCKET;
		MoveSource.close_flag.store(false, std::memory_order_release);
	}

	star::udp_socket::udp_socket(std::string ip, unsigned short port, star::ip_type IP_type)
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
			this->socket_client = socket(AF_INET, SOCK_DGRAM, 0);
			this->address_len = sizeof(sockaddr_in);
			break;
		case star::ip_type::ipv6:
			ret = inet_pton(AF_INET6, ip.c_str(), &(ip_address.ipv6.sin6_addr));
			if (ret != 1) {
				throw net_exception("ip translation failed!\n");
			}
			ip_address.ipv6.sin6_family = AF_INET6;
			ip_address.ipv6.sin6_port = htons(port);
			this->socket_client = socket(AF_INET6, SOCK_DGRAM, 0);
			this->address_len = sizeof(sockaddr_in6);
			if (ret != 0) {
				this->close();
			}
			break;
		default:
			throw net_exception("undefined ip type!\n");
			break;
		}
	}
	int udp_socket::read(char* buffer, int len, int offset)
	{
		int ret = -1;
		ret = ::recvfrom(this->socket_client, buffer + offset, len, 0, (SOCKADDR*)&(this->ip_address), &(this->address_len));
		return 0;
	}
	int udp_socket::write(char* buffer, int len, int offset)
	{
		int ret = -1;
		ret = ::sendto(this->socket_client, buffer + offset, len, 0, (SOCKADDR*)&(this->ip_address), this->address_len);
		return ret;
	}
	void udp_socket::close()
	{
		this->close_flag.store(true, std::memory_order_release);
		::closesocket(this->socket_client);
	}
	void udp_socket_server::Initialize(unsigned short port)
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
	}
	udp_socket_server::udp_socket_server(std::string ip, unsigned short port, int connect_num, star::ip_type IP_type)
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
		int ret = ::recvfrom(this->socket_server, buffer + offset, len, 0, (SOCKADDR*)&(source.addr), &(source.addr_len));
		return std::pair<int, socket_addr_container>(ret, source);
	}
	int udp_socket_server::write(socket_addr_container destination, char* buffer, int len, int offset)
	{
		socket_addr_container source{};
		int ret = ::sendto(this->socket_server, buffer + offset, len, 0, (SOCKADDR*)&(destination.addr), destination.addr_len);
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
}

#elif __linux__
namespace star {
	
}
#endif // WIN32
