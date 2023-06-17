#include"net_tool/udp_socket.h"
#include<iostream>
using namespace std;
int main() {
	try
	{
		star::udp_socket_server server_socket(8888);
		char buffer[2048];
		pair<int, star::socket_addr_container> read_msg = server_socket.read(buffer, 2048);
		{
			DWORD error = GetLastError();
			LPSTR messageBuffer = nullptr;
			size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, nullptr);
			std::cout << "socket failed with error: " << error << " - " << messageBuffer << std::endl;
			LocalFree(messageBuffer);
		}
		cout << read_msg.second.getAddr_Str() << ":" << read_msg.second.getPort() << endl;
		cout << buffer;
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
}