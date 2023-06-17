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
		std::atomic<bool> close_flag;		//�ر�flag
		std::atomic<bool> block_flag;		//������־λ
		std::shared_ptr<socket_addr_container> ip_address;	//��װ��ַ������
		star_socket star_socket_handle;		//socket���
		tcp_socket();						//����Ĭ�Ϲ��캯������Ԫ��tcp_socket_serverʹ��
	public:
		tcp_socket(tcp_socket&& MoveSource) noexcept;	//�ƶ����캯��
		tcp_socket(const tcp_socket&) = delete;			//ɾ��Ĭ�Ͽ�������
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
		* �ͷŵ�ַ����
		*/
		void freeAddr();
		/*
		* ��ȡ��ַ����
		*/
		std::shared_ptr<socket_addr_container> getAddr();
	public:
		/*
		* ��Ԫ��
		*/
		friend class tcp_socket_server;
	};

	class tcp_socket_server {
	private:
		int connect_num;
		tcp_socket::star_socket star_socket_handle;
		std::shared_ptr<socket_addr_container> ip_address;	//��װ��ַ������
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
		* �ͷŵ�ַ����
		*/
		void freeAddr();
		/*
		* ��ȡ��ַ����
		*/
		std::shared_ptr<socket_addr_container> getAddr();
		/*
		* ��ȡ��ַ�ַ���
		*/
		std::string getAddr_Str();
	public:
		/*
		* ��Ԫ��
		*/
	};
}


