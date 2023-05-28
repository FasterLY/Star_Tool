#pragma once
#include"net_shared.h"
namespace star {

	
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
		star_socket star_socket_handle;							//socket句柄
		std::shared_ptr<socket_addr_container> ip_address;		//连接状态暂时存储的socket地址信息
		friend class udp_socket_server;							//友元类
	public:
		udp_socket() = delete;									//删除默认构造函数
		udp_socket(star::ip_type IP_type);						//无连接状态的udp套接字。
		udp_socket(udp_socket&& MoveSource) noexcept;			//移动构造函数
		~udp_socket();
		udp_socket(const udp_socket&) = delete;					//删除默认拷贝函数
		udp_socket(std::string ip_or_domain, unsigned short port);		//创建有连接状态udp套接字
		int readfrom(socket_addr_container& address_buffer, char* buffer, int len, int offset = 0);
		int writefor(socket_addr_container& destination, char* buffer, int len, int offset = 0);
		int read(char* buffer, int len, int offset = 0);
		int write(char* buffer, int len, int offset = 0);
		void close();
		bool isClose();
		bool isConnected();
		/*
		* 释放地址容器
		*/
		void freeAddr();
		/*
		* 获取地址容器
		*/
		std::shared_ptr<socket_addr_container> getAddr();
	};

	class udp_socket_server {
	private:
		std::atomic<bool> close_flag;
		udp_socket::star_socket star_socket_handle;
		std::shared_ptr<socket_addr_container> ip_address;	//填装接收报文的本地地址的容器
	public:
		udp_socket_server(std::string ip_or_domain, unsigned short port);
		udp_socket_server(short port, star::ip_type IP_type = star::ip_type::ipv4);
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
		/*
		* 释放地址容器
		*/
		void freeAddr();
		/*
		* 获取地址容器
		*/
		std::shared_ptr<socket_addr_container> getAddr();
	};
}
