//Client

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include "Buffer.h"
#include "MessageProtocol.h"

SOCKET Connection;

void clientThread()
{
	char buffer[512];
	Buffer*(myBuffer) = new Buffer(512);

	MessageProtocol* messageProtocol = new MessageProtocol();
	while (true)
	{
		//Read 2 ints first, then create new buffer for the remaining message and read the rest. According to received command perform 
		//an action. we can add some stop anchor, if reached it the end of package
		recv(Connection, (char*)(myBuffer->mBuffer[0]), myBuffer->mBuffer.size(), NULL);
		std::cout << buffer << std::endl;
	}
}


int main()
{
	//Winsock Startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBox(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	//Setting Socket address
	SOCKADDR_IN addr;
	int sizeofadr = sizeof(addr);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET; //IPv4 

	Connection = socket(AF_INET, SOCK_STREAM, NULL); //Creates connection socket
	if (connect(Connection, (SOCKADDR*)&addr, sizeofadr) != 0)
	{
		MessageBox(NULL, "Fainled to connect", "Error", MB_OK | MB_ICONERROR);
	}
	std::cout << "Connected!" << std::endl;
	char buffer[256];
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientThread, NULL, NULL, NULL); //Create a thread

	while (true)
	{
		std::cin.getline(buffer, sizeof(buffer));
		send(Connection, buffer, sizeof(buffer), NULL);
		Sleep(10);
	}

	system("pause");
	return 0;
}