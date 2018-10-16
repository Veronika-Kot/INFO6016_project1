//Server
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include "MessageProtocol.h"
#include <vector>
#include <string>

struct client
{
	SOCKET Connection;
	std::string name = "";
	std::string room = "";
};

client Clients[100];
int clientsCounter = 0;
void sendMessageToClient(SOCKET theConnection, int id, std::string message);
void sendMessageToAllInGroup(std::string groupName, int id, std::string message);
void sendMessageOthersInGroup(int clientIndex, std::string groupName, int id, std::string message);
void handleClients(int index)
{
	int packLength;
	std::string name = "";
	while (true)
	{
		std::vector<char> packet(256);
		if ((packLength = recv(Clients[index].Connection, &packet[0], packet.size(), NULL)) > 0);
		{
			MessageProtocol* messageProtocol = new MessageProtocol();
			messageProtocol->createBuffer(256);

			messageProtocol->buffer->mBuffer = packet;
			messageProtocol->readHeader(*messageProtocol->buffer);
			messageProtocol->receiveMessage(*messageProtocol->buffer);

			if(messageProtocol->messageHeader.command_id == 0) //create name
			{
				Clients[index].name = messageProtocol->messageBody.message;
				std::string greet = "Nice to meet you, " + messageProtocol->messageBody.message + "!";
				sendMessageToClient(Clients[index].Connection, 0, greet);

				std::string setGroup = "\nWhich group would you like to join? Send me a number!\n"
				"1 - Movies, 2 - Games, 3 - Sport"; //can be stored seperatly in the feature
				sendMessageToClient(Clients[index].Connection, 2, setGroup);

				continue;
			}

			if (messageProtocol->messageHeader.command_id == 2) //join the room
			{
				std::cout << "I am here " << std::endl;
				if (messageProtocol->messageBody.message == "1")
				{
					std::cout << "I am in movies " << std::endl;
					Clients[index].room = "Movies";
					std::string message = "*" + Clients[index].name + "* joined the room Movies.";
					sendMessageToAllInGroup("Movies", 1, message);
					continue;
				}
				else if (messageProtocol->messageBody.message == "2")
				{
					Clients[index].room = "Games";
					std::string message = "*" + Clients[index].name + "* joined the room Games.";
					sendMessageToAllInGroup("Games", 1, message);
					continue;
				}
				else if (messageProtocol->messageBody.message == "3")
				{
					Clients[index].room = "Sport";
					std::string message = "*" + Clients[index].name + "* joined the room Sport.";
					sendMessageToAllInGroup("Sport", 1, message);
					continue;
				}
				else {
					std::string setGroup = "\nWrong group number! Try again!\n"
						"1 - Movies, 2 - Games, 3 - Sport"; //can be stored seperatly in the feature
					sendMessageToClient(Clients[index].Connection, 2, setGroup);
					continue;
				}
			}

			if (messageProtocol->messageHeader.command_id == 1)
			{
				std::cout << Clients[index].name << ": " << messageProtocol->messageBody.message << std::endl;
				std::string message = Clients[index].name + ">> " + messageProtocol->messageBody.message;
				sendMessageOthersInGroup(index, Clients[index].room, 1, message);
			}

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

			sendMessageToClient(newConnection, 0, "Welcome to the Fan chat!\nWhat is your name?");

			
			Clients[i].Connection = newConnection;
			//Connections[i] = newConnection;
			clientsCounter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)handleClients, (LPVOID)(i), NULL, NULL); //Create a thread
		}
	}
	system("pause");
	return 0;
}

void sendMessageToClient(SOCKET theConnection, int id, std::string message)
{
	MessageProtocol* messageSendProtocol = new MessageProtocol();
	messageSendProtocol->messageHeader.command_id = id;
	messageSendProtocol->messageBody.message = message;
	messageSendProtocol->createBuffer(4);
	messageSendProtocol->sendMessage(*messageSendProtocol->buffer);

	std::vector<char> packet = messageSendProtocol->buffer->mBuffer;

	send(theConnection, &packet[0], packet.size(), 0);

	delete messageSendProtocol;
}

void sendMessageToAllInGroup(std::string groupName, int id, std::string message)
{
	MessageProtocol* messageSendProtocol = new MessageProtocol();
	messageSendProtocol->messageHeader.command_id = id;

	messageSendProtocol->messageBody.message = message;
	messageSendProtocol->createBuffer(4);
	messageSendProtocol->sendMessage(*messageSendProtocol->buffer);
	std::vector<char> packet = messageSendProtocol->buffer->mBuffer;
	for (int i = 0; i < clientsCounter; i++)
	{
		if (Clients[i].room == groupName)
		{
			send(Clients[i].Connection, &packet[0], packet.size(), 0);
		}
		else 
		{
			continue;
		}
	}
	delete messageSendProtocol;
}

void sendMessageOthersInGroup(int clientIndex,  std::string groupName, int id, std::string message)
{
	MessageProtocol* messageSendProtocol = new MessageProtocol();
	messageSendProtocol->messageHeader.command_id = id;

	messageSendProtocol->messageBody.message = message;
	messageSendProtocol->createBuffer(4);
	messageSendProtocol->sendMessage(*messageSendProtocol->buffer);
	std::vector<char> packet = messageSendProtocol->buffer->mBuffer;
	for (int i = 0; i < clientsCounter; i++)
	{
		if (clientIndex == i) 
		{
			continue;
		}
		if (Clients[i].room == groupName)
		{
			send(Clients[i].Connection, &packet[0], packet.size(), 0);
		}
	}
	delete messageSendProtocol;
}