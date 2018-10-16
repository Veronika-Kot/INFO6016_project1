//Server
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include "MessageProtocol.h"
#include <vector>

SOCKET Connections[100];
int connectionCounter = 0;

void handleClients(int index)
{
	int packLength;
	while (true)
	{
		std::vector<char> packet(256);
		if ((packLength = recv(Connections[index], &packet[0], packet.size(), NULL)) > 0);
		{
			MessageProtocol* messageProtocol = new MessageProtocol();
			messageProtocol->createBuffer(256);

			messageProtocol->buffer->mBuffer = packet;
			messageProtocol->readHeader(*messageProtocol->buffer);
			messageProtocol->receiveMessage(*messageProtocol->buffer);
			std::cout << "Client #" << index << ": " << messageProtocol->messageBody.message << std::endl;
			/*	if (messageProtocol->messageHeader.command_id == 1)
				{*/

			for (int i = 0; i < connectionCounter; i++)
			{
				if (i == index)
				{
					continue;
				}
				else
				{
					packet.resize(messageProtocol->messageHeader.packet_length);
					send(Connections[i], &packet[0], packet.size(), 0);
				}
			}
			//}

			packLength = 0;
			packet.clear();
			delete messageProtocol;
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

			MessageProtocol* messageSendProtocol = new MessageProtocol();
			messageSendProtocol->messageHeader.command_id = 001;
			messageSendProtocol->messageBody.message = "Welcome! this is a message from the server";
			messageSendProtocol->createBuffer(4);
			messageSendProtocol->sendMessage(*messageSendProtocol->buffer);


			std::vector<char> packet = messageSendProtocol->buffer->mBuffer;

			send(newConnection, &packet[0], packet.size(), 0);
						
			Connections[i] = newConnection;
			connectionCounter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)handleClients, (LPVOID)(i), NULL, NULL); //Create a thread
		}
	}
	system("pause");
	return 0;
}