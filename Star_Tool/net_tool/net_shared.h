#pragma once
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
#include<memory>
#include<atomic>
#include<mutex>
#include<string>
#include<string.h>
#include<iostream>
namespace star {
	/*
	* ip协议族枚举类型：ipv4，ipv6
	*/
	enum class ip_type :char {
		ipv4,
		ipv6
	};
	/*
	* 传输层协议流枚举类型：tcp，udp
	*/
	enum class tran_type :char {	//传输层协议流类型
		udp,
		tcp
	};
	/*
	* 网络抛出错误类型
	*/
	class net_exception :public std::exception {
	private:
		std::string error_msg;
	public:
		net_exception(std::string error_msg);
		virtual const char* what() const throw() override;
	};
	class socket_addr_container {
	private:
		/*
		* 地址联合体
		*/
		union socket_addr {
			sockaddr_in ipv4;
			sockaddr_in6 ipv6;
		};
	private:
		socket_addr ip_address;
		socklen_t addr_len;
		ip_type ip_protocol;
	public:
		socket_addr_container();
		socket_addr_container(unsigned short port, ip_type ip_protocol);
		socket_addr_container(std::string domain);
		socket_addr_container(std::string ip_or_domain, unsigned short port);
		socket_addr_container(socket_addr_container&& MoveSource)noexcept;
		socket_addr_container(const socket_addr_container&) = default;
		friend class tcp_socket_server;
		friend class tcp_socket;
		friend class udp_socket_server;
		friend class udp_socket;
	};
}