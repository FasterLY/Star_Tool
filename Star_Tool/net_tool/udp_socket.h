#pragma once
#include"net_shared.h"
#ifdef _WIN32
	namespace star {

		class socket_addr_container {
		private:
			socket_addr addr;
			int addr_len;
		public:
			friend class udp_socket_server;
			friend class udp_socket;
		};

		extern std::atomic<bool> isInitialize;
		class udp_socket_server;
		class udp_socket {
		private:
			std::atomic<bool> close_flag;		//关闭flag
			SOCKET socket_client;				//socket句柄
			socket_addr ip_address;				//socket地址信息
			int address_len = sizeof(SOCKADDR);	//socket地址信息长度
			star::ip_type IP_type;				//socket种类
			friend class udp_socket_server;		//友元类
			udp_socket();						//保留默认构造函数给友元类udp_socket_server使用
		public:
			udp_socket(udp_socket&& MoveSource) noexcept;	//移动构造函数
			udp_socket(const udp_socket&) = delete;			//删除默认拷贝函数
			udp_socket(std::string ip, unsigned short port, star::ip_type IP_type = star::ip_type::ipv4);
			int read(char* buffer, int len, int offset = 0);
			int write(char* buffer, int len, int offset = 0);
			void close();
		};

		class udp_socket_server {
		private:
			std::atomic<bool> close_flag;
			SOCKET socket_server;
			socket_addr ip_address;
			int address_len = sizeof(SOCKADDR);
			star::ip_type IP_type;
			void Initialize(unsigned short port);
		public:
			udp_socket_server(std::string ip, unsigned short port,
				int connect_num = SOMAXCONN,
				star::ip_type IP_type = star::ip_type::ipv4);
			udp_socket_server(short port, int connect_num = SOMAXCONN,
				star::ip_type IP_type = star::ip_type::ipv4);
			unsigned short getPort();
			std::pair<int, socket_addr_container> read(char* buffer, int len, int offset = 0);
			int write(socket_addr_container destination, char* buffer, int len, int offset = 0);
			void close();
			bool isClose();
		};
	}
	
#elif __linux__
	namespace star {
		class udp_socket_server {
		private:
			
		public:
			
		};
		class udp_socket {
		private:
			
		public:
			
		};
	}
#endif // WIN32
