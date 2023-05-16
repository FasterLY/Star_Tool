#pragma once
#include"net_shared.h"
#include<string>
#ifdef _WIN32
namespace star {
	extern std::atomic<bool> isInitialize;
	union socket_addr {
		sockaddr_in ipv4;
		sockaddr_in6 ipv6;
	};
	class tcp_socket_server;
	class tcp_socket {
	private:
		std::atomic<bool> close_flag;		//关闭flag
		SOCKET socket_client;				//socket句柄
		socket_addr ip_address;				//socket地址信息
		int address_len = sizeof(SOCKADDR);	//socket地址信息长度
		star::ip_type IP_type;				//socket种类
		friend class tcp_socket_server;		//友元类
		tcp_socket();						//保留默认构造函数给友元类tcp_socket_server使用
	public:
		tcp_socket(tcp_socket&& MoveSource) noexcept;	//移动构造函数
		tcp_socket(const tcp_socket&) = delete;			//删除默认拷贝函数
		tcp_socket(std::string ip, unsigned short port, star::ip_type IP_type = star::ip_type::ipv4);
		int read(char* buffer, int len, int offset = 0);
		int write(char* buffer, int len, int offset = 0);
		void close();
	};

	class tcp_socket_server {
	private:
		std::atomic<bool> close_flag;
		SOCKET socket_server;
		socket_addr ip_address;
		int address_len = sizeof(SOCKADDR);
		star::ip_type IP_type;
		int connect_num;
		void Initialize(unsigned short port);
	public:
		tcp_socket_server(std::string ip, unsigned short port,
			int connect_num = SOMAXCONN,
			star::ip_type IP_type = star::ip_type::ipv4);
		tcp_socket_server(short port, int connect_num = SOMAXCONN,
			star::ip_type IP_type = star::ip_type::ipv4);
		tcp_socket accept();
		unsigned short getPort();
		void close();
		bool isClose();
	};
}

#elif  __linux__
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
namespace star {
	union socket_addr {
		sockaddr_in ipv4;
		sockaddr_in6 ipv6;
	};
	class tcp_socket_server;
	class tcp_socket {
	private:
		std::atomic<bool> close_flag;		//关闭flag
		int socket_client;				//socket句柄
		socket_addr ip_address;				//socket地址信息
		socklen_t address_len = sizeof(sockaddr);	//socket地址信息长度
		star::ip_type IP_type;				//socket种类
		friend class tcp_socket_server;
		tcp_socket();						//保留默认构造函数给友元类tcp_socket_server使用
	public:
		tcp_socket(tcp_socket&& MoveSource) noexcept;	//移动构造函数
		tcp_socket(const tcp_socket&) = delete;			//删除默认拷贝函数
		tcp_socket(std::string ip, unsigned short port, star::ip_type IP_type = star::ip_type::ipv4);
		int read(char* buffer, int len, int offset = 0);
		int write(char* buffer, int len, int offset = 0);
		void close();
	};
	class tcp_socket_server {
	private:
		std::atomic<bool> close_flag;
		int socket_server;
		socket_addr ip_address;
		socklen_t address_len = sizeof(sockaddr);
		star::ip_type IP_type;
		int connect_num;
		void Initialize(unsigned short port);
	public:
		tcp_socket_server(std::string ip, unsigned short port,
			int connect_num = SOMAXCONN,
			star::ip_type IP_type = star::ip_type::ipv4);
		tcp_socket_server(short port, int connect_num = SOMAXCONN,
			star::ip_type IP_type = star::ip_type::ipv4);
		tcp_socket accept();
		unsigned short getPort();
		void close();
		bool isClose();
	};
}
#endif // WIN32


