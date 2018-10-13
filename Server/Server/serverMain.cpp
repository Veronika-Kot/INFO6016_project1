//Server
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>

SOCKET Connections[100];
int connectionCounter = 0;

void handleClients(int index)
{
	char buffer[256];
	while (true)
	{
		recv(Connections[index], buffer, sizeof(buffer), NULL);
		for (int i = 0; i < connectionCounter; i++)
		{
			if (i == index)
			{
				continue;
			}
			else {
				send(Connections[i], buffer, sizeof(buffer), NULL);
			}
		}
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

	//Socket addres info

	SOCKADDR_IN addr;
	//int addrlen = sizeof(addr);
	//addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//addr.sin_port = htons(1111);
	//addr.sin_family = AF_INET; //IPv4 Socket

	int addrlen = sizeof(addr);
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET; //IPv4 


	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //Creates socket to listen for new connections
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr)); // Binds the address to the socket
	listen(sListen, SOMAXCONN); //Listen the socket

	//To accept a connection
	SOCKET newConnection; //Socket to hold the client's connection
	for (int i = 0; i < 100; i++)
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen);
		if (newConnection == 0)
		{
			std::cout << "Failed to accept the clien's connection." << std::endl;

		}
		else
		{
			std::cout << "Client Connected!" << std::endl;
			char MOTD[256] = "Welcome! this is a message from the server"; //Creates a buffer
			send(newConnection, MOTD, sizeof(MOTD), NULL); // Send buffer
			Connections[i] = newConnection;
			connectionCounter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)handleClients, (LPVOID)(i), NULL, NULL); //Create a thread
		}
	}
	system("pause");
	return 0;
}