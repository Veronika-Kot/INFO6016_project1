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
	MessageProtocol* messageProtocol = new MessageProtocol();
	messageProtocol->createBuffer(256);
	std::vector<char> packet(256);
	while (true)
	{
		recv(Connection, &packet[0], packet.size(), NULL);
		messageProtocol->buffer->mBuffer = packet;
		messageProtocol->readHeader(*messageProtocol->buffer);

		messageProtocol->buffer->resizeBuffer(messageProtocol->messageHeader.packet_length);
		messageProtocol->receiveMessage(*messageProtocol->buffer);
		std::cout<< messageProtocol->messageBody.message<<std::endl;
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

	MessageProtocol* messageProtocol = new MessageProtocol();
	messageProtocol->createBuffer(256);

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientThread, NULL, NULL, NULL); //Create a thread

	MessageProtocol* messageSendProtocol = new MessageProtocol();
	messageProtocol->messageHeader.command_id = 001;

	while (true)
	{
		std::string input = "";
		std::getline(std::cin, input);
		messageSendProtocol->messageBody.message = input.c_str();
		messageSendProtocol->createBuffer(8);
		messageSendProtocol->sendMessage(*messageSendProtocol->buffer);
		std::vector<char> packet = messageSendProtocol->buffer->mBuffer;

		send(Connection, &packet[0], packet.size(), 0);

		Sleep(10);
	}

	system("pause");
	return 0;
}