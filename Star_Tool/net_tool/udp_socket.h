#pragma once
#include"net_shared.h"
namespace star {

	class socket_addr_container {
	private:
		socket_addr ip_address;
		socklen_t addr_len;
	public:
		socket_addr_container();
		socket_addr_container(std::string ip, unsigned short port, star::ip_type IP_type = star::ip_type::ipv4);
		socket_addr_container(socket_addr_container&& MoveSource)noexcept;
		socket_addr_container(const socket_addr_container&) = default;
		friend class udp_socket_server;
		friend class udp_socket;
	};
	class udp_socket_server;
	class udp_socket {
	private:
#ifdef _WIN32
		using star_sockaddr = SOCKADDR;
		using star_socket = SOCKET;
#elif __linux__
		using star_sockaddr = sockaddr;
		using star_socket = int;
#endif
	private:
		std::atomic<bool> connect_flag;							//连接状态flag
		std::atomic<bool> close_flag;							//关闭flag
		star_socket star_socket_handle;								//socket句柄
		std::unique_ptr<socket_addr> ip_address;				//socket地址信息
		socklen_t address_len = sizeof(star_sockaddr);			//socket地址信息长度
		star::ip_type IP_type;									//socket种类
		friend class udp_socket_server;							//友元类
	public:
		udp_socket() = delete;									//删除默认构造函数
		udp_socket(star::ip_type IP_type);						//无连接状态的udp套接字。
		udp_socket(udp_socket&& MoveSource) noexcept;			//移动构造函数
		~udp_socket();
		udp_socket(const udp_socket&) = delete;					//删除默认拷贝函数
		udp_socket(std::string ip, unsigned short port, star::ip_type IP_type = star::ip_type::ipv4);	//创建有连接状态udp套接字
		int readfrom(socket_addr_container& address_buffer, char* buffer, int len, int offset = 0);
		int writefor(socket_addr_container& destination, char* buffer, int len, int offset = 0);
		int read(char* buffer, int len, int offset = 0);
		int write(char* buffer, int len, int offset = 0);
		void close();
		bool isClose();
		bool isConnected();
	};

	class udp_socket_server {
	private:
		std::atomic<bool> close_flag;
		udp_socket::star_socket star_socket_handle;
		socket_addr ip_address;
		socklen_t address_len = sizeof(udp_socket::star_sockaddr);
		star::ip_type IP_type;
		void Initialize(unsigned short port);
	public:
		udp_socket_server(std::string ip, unsigned short port,
			int connect_num = SOMAXCONN,
			star::ip_type IP_type = star::ip_type::ipv4);
		udp_socket_server(short port, int connect_num = SOMAXCONN,
			star::ip_type IP_type = star::ip_type::ipv4);
		~udp_socket_server();
		unsigned short getPort();
		/*
		* 服务端仅有两个读写方法且不保留连接状态，服务端应该根据返回的地址写回响应数据
		*/
		std::pair<int, socket_addr_container> read(char* buffer, int len, int offset = 0);
		/*
		* 服务端仅有两个读写方法且不保留连接状态，服务端应该根据返回的地址写回响应数据
		*/
		int write(socket_addr_container& destination, char* buffer, int len, int offset = 0);
		void close();
		bool isClose();
	};
}
