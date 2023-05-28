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
		std::atomic<bool> connect_flag;							//����״̬flag
		std::atomic<bool> close_flag;							//�ر�flag
		star_socket star_socket_handle;							//socket���
		std::shared_ptr<socket_addr_container> ip_address;		//����״̬��ʱ�洢��socket��ַ��Ϣ
		friend class udp_socket_server;							//��Ԫ��
	public:
		udp_socket() = delete;									//ɾ��Ĭ�Ϲ��캯��
		udp_socket(star::ip_type IP_type);						//������״̬��udp�׽��֡�
		udp_socket(udp_socket&& MoveSource) noexcept;			//�ƶ����캯��
		~udp_socket();
		udp_socket(const udp_socket&) = delete;					//ɾ��Ĭ�Ͽ�������
		udp_socket(std::string ip_or_domain, unsigned short port);		//����������״̬udp�׽���
		int readfrom(socket_addr_container& address_buffer, char* buffer, int len, int offset = 0);
		int writefor(socket_addr_container& destination, char* buffer, int len, int offset = 0);
		int read(char* buffer, int len, int offset = 0);
		int write(char* buffer, int len, int offset = 0);
		void close();
		bool isClose();
		bool isConnected();
		/*
		* �ͷŵ�ַ����
		*/
		void freeAddr();
		/*
		* ��ȡ��ַ����
		*/
		std::shared_ptr<socket_addr_container> getAddr();
	};

	class udp_socket_server {
	private:
		std::atomic<bool> close_flag;
		udp_socket::star_socket star_socket_handle;
		std::shared_ptr<socket_addr_container> ip_address;	//��װ���ձ��ĵı��ص�ַ������
	public:
		udp_socket_server(std::string ip_or_domain, unsigned short port);
		udp_socket_server(short port, star::ip_type IP_type = star::ip_type::ipv4);
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
		/*
		* �ͷŵ�ַ����
		*/
		void freeAddr();
		/*
		* ��ȡ��ַ����
		*/
		std::shared_ptr<socket_addr_container> getAddr();
	};
}
