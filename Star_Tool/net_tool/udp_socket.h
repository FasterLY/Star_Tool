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
		std::atomic<bool> connect_flag;							//����״̬flag
		std::atomic<bool> close_flag;							//�ر�flag
		star_socket star_socket_handle;								//socket���
		std::unique_ptr<socket_addr> ip_address;				//socket��ַ��Ϣ
		socklen_t address_len = sizeof(star_sockaddr);			//socket��ַ��Ϣ����
		star::ip_type IP_type;									//socket����
		friend class udp_socket_server;							//��Ԫ��
	public:
		udp_socket() = delete;									//ɾ��Ĭ�Ϲ��캯��
		udp_socket(star::ip_type IP_type);						//������״̬��udp�׽��֡�
		udp_socket(udp_socket&& MoveSource) noexcept;			//�ƶ����캯��
		~udp_socket();
		udp_socket(const udp_socket&) = delete;					//ɾ��Ĭ�Ͽ�������
		udp_socket(std::string ip, unsigned short port, star::ip_type IP_type = star::ip_type::ipv4);	//����������״̬udp�׽���
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
		* ����˽���������д�����Ҳ���������״̬�������Ӧ�ø��ݷ��صĵ�ַд����Ӧ����
		*/
		std::pair<int, socket_addr_container> read(char* buffer, int len, int offset = 0);
		/*
		* ����˽���������д�����Ҳ���������״̬�������Ӧ�ø��ݷ��صĵ�ַд����Ӧ����
		*/
		int write(socket_addr_container& destination, char* buffer, int len, int offset = 0);
		void close();
		bool isClose();
	};
}
