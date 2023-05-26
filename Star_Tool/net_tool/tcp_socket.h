#pragma once
#include"net_shared.h"
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
		star_socket star_socket_handle;			//socket���
		socket_addr ip_address;				//socket��ַ��Ϣ
		socklen_t address_len = sizeof(star_sockaddr);	//socket��ַ��Ϣ����
		star::ip_type IP_type;				//socket����
		friend class tcp_socket_server;		//��Ԫ��
		tcp_socket();						//����Ĭ�Ϲ��캯������Ԫ��tcp_socket_serverʹ��
	public:
		tcp_socket(tcp_socket&& MoveSource) noexcept;	//�ƶ����캯��
		tcp_socket(const tcp_socket&) = delete;			//ɾ��Ĭ�Ͽ�������
		tcp_socket(std::string ip, unsigned short port, star::ip_type IP_type = star::ip_type::ipv4);
		int read(char* buffer, int len, int offset = 0);
		int write(char* buffer, int len, int offset = 0);
		void close();
		bool isClose();
	};

	class tcp_socket_server {
	private:
		std::atomic<bool> close_flag;
		tcp_socket::star_socket star_socket_handle;
		socket_addr ip_address;
		socklen_t address_len = sizeof(tcp_socket::star_sockaddr);
		star::ip_type IP_type;
		int connect_num;
		void Initialize(unsigned short port);
	public:
		tcp_socket_server(std::string ip, unsigned short port,
			int connect_num = SOMAXCONN,
			star::ip_type IP_type = star::ip_type::ipv4);
		tcp_socket_server(unsigned short port, int connect_num = SOMAXCONN,
			star::ip_type IP_type = star::ip_type::ipv4);
		tcp_socket accept();
		unsigned short getPort();
		void close();
		bool isClose();
	};
}


