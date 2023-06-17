#pragma once
#include"net_shared.h"
#include"star_poll.h"
#include<string>
namespace star {
	class tcp_socket_server;
	class tcp_socket {
	private:
#ifdef _WIN32
		using star_sockaddr = SOCKADDR;
		using star_socket = SOCKET;
#elif __linux__
		using star_sockaddr = sockaddr;
		using star_socket = int;
#endif
	private:
		std::atomic<bool> close_flag;		//关闭flag
		std::atomic<bool> block_flag;		//阻塞标志位
		std::shared_ptr<socket_addr_container> ip_address;	//填装地址的容器
		star_socket star_socket_handle;		//socket句柄
		tcp_socket();						//保留默认构造函数给友元类tcp_socket_server使用
	public:
		tcp_socket(tcp_socket&& MoveSource) noexcept;	//移动构造函数
		tcp_socket(const tcp_socket&) = delete;			//删除默认拷贝函数
		tcp_socket(std::string ip_or_domain, unsigned short port);
		tcp_socket(std::string domain_name);
		~tcp_socket();
		int read(char* buffer, int len, int offset = 0);
		int write(char* buffer, int len, int offset = 0);
		bool setblock(bool block);
		bool isblock();
		int availavle();
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
	public:
		/*
		* 友元类
		*/
		friend class tcp_socket_server;
	};

	class tcp_socket_server {
	private:
		int connect_num;
		tcp_socket::star_socket star_socket_handle;
		std::shared_ptr<socket_addr_container> ip_address;	//填装地址的容器
		std::atomic<bool> close_flag;
	public:
		tcp_socket_server(std::string ip_or_domain, unsigned short port,int connect_num = SOMAXCONN);
		tcp_socket_server(unsigned short port,star::ip_type IP_type = star::ip_type::ipv4, int connect_num = SOMAXCONN);
		~tcp_socket_server();
		tcp_socket accept();
		unsigned short getPort();
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
		/*
		* 获取地址字符串
		*/
		std::string getAddr_Str();
	public:
		/*
		* 友元类
		*/
	};
}


