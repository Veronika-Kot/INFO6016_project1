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
#include "../gen/chat.pb.h"

struct client
{
	SOCKET Connection;
	SOCKET ConnectionToDS;
	std::string name = "";
	std::string room = "";
	std::string email = "";
	std::string authID = "";
	int id;
};
//SOCKET ConnectionToDataServer;
int iResult;
client Clients[100];
int clientsCounter = 1;
void sendMessageToClient(SOCKET theConnection, int id, std::string message);
void sendMessageToAllInGroup(std::string groupName, int id, std::string message);
void sendMessageOthersInGroup(int clientIndex, std::string groupName, int id, std::string message);
void connectToDataServer(int index);

void handleClients(int index)
{
	int packLength;
	std::string name = "";
	bool run = true;
	while (run)
	{
		std::vector<char> packet(512);
		if ((packLength = recv(Clients[index].Connection, &packet[0], packet.size(), NULL)) < 1) {
			closesocket(Clients[index].Connection);
			//WSACleanup();
			std::cout << "Buffer length is less then 1" << std::endl;
			run = false;
		}
		else
		{
			MessageProtocol* messageProtocol = new MessageProtocol();
			messageProtocol->createBuffer(512);

			messageProtocol->buffer->mBuffer = packet;
			messageProtocol->readHeader(*messageProtocol->buffer);
			

			if (messageProtocol->messageHeader.command_id == 4) //ask for email
			{
				messageProtocol->receiveMessage(*messageProtocol->buffer);
				Clients[index].authID = messageProtocol->messageBody.message;
				std::cout << messageProtocol->messageBody.message << std::endl;
				if (Clients[index].authID == "1" || Clients[index].authID == "2")
				{
					sendMessageToClient(Clients[index].Connection, 5, "Send me your email");
				}
				else 
				{
					sendMessageToClient(Clients[index].Connection, 4, "Wrong number! Try Again!\n1-Login, 2-Create Account (send me a number)");
					Clients[index].authID = "";
				}
			}

			if (messageProtocol->messageHeader.command_id == 5) //ask for password
			{
				messageProtocol->receiveMessage(*messageProtocol->buffer);
				Clients[index].email = messageProtocol->messageBody.message;
				sendMessageToClient(Clients[index].Connection, 6, "Send me your password");

			}

			if (messageProtocol->messageHeader.command_id == 6) //connect to auth server
			{
				messageProtocol->receiveMessage(*messageProtocol->buffer);
				std::string password = messageProtocol->messageBody.message;

				ClientMessageWrapper clientMasterMessage;
				clientMasterMessage.set_clientindex(index);
				
				if (Clients[index].authID == "2")
				{
					CreateAccountWeb* createAccountWeb = new CreateAccountWeb();
					createAccountWeb->set_email(Clients[index].email);
					createAccountWeb->set_plaintextpassword(password);
					clientMasterMessage.set_messageid(1);
					clientMasterMessage.set_allocated_m1(createAccountWeb);
				} 

				if (Clients[index].authID == "1")
				{
					AuthenticateWeb* createAccountWeb = new AuthenticateWeb();
					createAccountWeb->set_email(Clients[index].email);
					createAccountWeb->set_plaintextpassword(password);
					clientMasterMessage.set_messageid(2);
					clientMasterMessage.set_allocated_m2(createAccountWeb);
				}

				std::cout << clientMasterMessage.DebugString() << std::endl;
				size_t size = clientMasterMessage.ByteSizeLong();
				char bufferServerMessage[512];

				std::string bufferString;
				clientMasterMessage.SerializeToString(&bufferString);
				strcpy(bufferServerMessage, bufferString.c_str());
				iResult = send(Clients[index].ConnectionToDS, &bufferServerMessage[0], 512, 0);
				if (iResult == SOCKET_ERROR) {
					printf("send failed: %d\n", WSAGetLastError());
				}
			}

			if(messageProtocol->messageHeader.command_id == 0) //create name
			{
				messageProtocol->receiveName(*messageProtocol->buffer);
				std::string greet;
				if (Clients[index].name == "")
				{
					Clients[index].name = messageProtocol->messageBody.message;
					greet = "Nice to meet you, " + Clients[index].name + "!";
				}
				sendMessageToClient(Clients[index].Connection, 0, greet);

				std::string setGroup = "\nWhich room would you like to join? Send me a number!\n"
					"1-Movies, 2-Games, 3-Sport (to leave room send leaveRoom)"; //rooms will be stored seperatly in the feature
				sendMessageToClient(Clients[index].Connection, 2, setGroup);

				continue;
			}

			if (messageProtocol->messageHeader.command_id == 3) //leave the room
			{
				std::string message = "*" + Clients[index].name + "* left the room";
				sendMessageToAllInGroup(Clients[index].room, 1, message);
				Clients[index].room = "";

				std::string setGroup = "\nWhich room would you like to join? Send me a number!\n"
					"1-Movies, 2-Games, 3-Sport (to leave room send leaveRoom)"; //rooms will be stored seperatly in the feature
				sendMessageToClient(Clients[index].Connection, 2, setGroup);

				continue;
				
			}

			if (messageProtocol->messageHeader.command_id == 7) //loggout
			{
				messageProtocol->receiveMessage(*messageProtocol->buffer);
				if (Clients[index].room != "")
				{
					std::string message = "*" + Clients[index].name + "* left the room";
					sendMessageToAllInGroup(Clients[index].room, 1, message);
					Clients[index].room = "";
				}

				sendMessageToClient(Clients[index].Connection, 4, "Welcome to the Fan chat!\n1-Login, 2-Create Account (send me a number)");
				

				continue;

			}

			if (messageProtocol->messageHeader.command_id == 2) //join the room
			{
				messageProtocol->joinRoom(*messageProtocol->buffer);
				if (messageProtocol->messageBody.roomName == "1")
				{
					Clients[index].room = "Movies";
					std::string message = "*" + Clients[index].name + "* has joined the room Movies.";
					sendMessageToAllInGroup("Movies", 1, message);
					continue;
				}
				else if (messageProtocol->messageBody.roomName == "2")
				{
					Clients[index].room = "Games";
					std::string message = "*" + Clients[index].name + "* has joined the room Games.";
					sendMessageToAllInGroup("Games", 1, message);
					continue;
				}
				else if (messageProtocol->messageBody.roomName == "3")
				{
					Clients[index].room = "Sport";
					std::string message = "*" + Clients[index].name + "* has joined the room Sport.";
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
				messageProtocol->receiveMessage(*messageProtocol->buffer);
				std::cout << messageProtocol->messageBody.name << ": " << messageProtocol->messageBody.message << std::endl;
				std::string message = messageProtocol->messageBody.name + ">> " + messageProtocol->messageBody.message;
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
	//sendRequestToDataServer();

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
	addr.sin_port = htons(1234567);
	addr.sin_family = AF_INET; //IPv4 

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //Creates socket to listen for new connections
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr)); // Binds the address to the socket
	listen(sListen, SOMAXCONN); //Listen the socket

	//To accept a connection
	SOCKET newConnection; //Socket to hold the client's connection
	for (int i = 1; i < 100; i++)
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen);
		if (newConnection == 0)
		{
			std::cout << "Failed to accept the clien's connection." << std::endl;

		}
		else
		{
			std::cout << "Client Connected!" << std::endl;
			sendMessageToClient(newConnection, 4, "Welcome to the Fan chat!\n1-Login, 2-Create Account (send me a number)");

			
			Clients[i].Connection = newConnection;
			//Connections[i] = newConnection;
			clientsCounter++;

			//Start socket to connect to data server
			connectToDataServer(i);
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)handleClients, (LPVOID)(i), NULL, NULL); //Create a thread

		}
	}
	system("pause");
	return 0;
}

//sendMessageOthersInGroup
//
//Purpouse: Sends server message to the curent conection
//
//@param: connection Id, ommand id, the message to be sent
//@return: void

void sendMessageToClient(SOCKET theConnection, int id, std::string message)
{
	MessageProtocol* messageSendProtocol = new MessageProtocol();
	messageSendProtocol->messageHeader.command_id = id;
	messageSendProtocol->messageBody.message = message;
	messageSendProtocol->createBuffer(4);
	messageSendProtocol->sendMessage(*messageSendProtocol->buffer, id);

	std::vector<char> packet = messageSendProtocol->buffer->mBuffer;

	send(theConnection, &packet[0], packet.size(), 0);

	delete messageSendProtocol;
}

//sendMessageOthersInGroup
//
//Purpouse: Send message to all clients in the chat room including the curent conection
//
//@param:  room name, command id, the message to be sent
//@return: void

void sendMessageToAllInGroup(std::string groupName, int id, std::string message)
{
	MessageProtocol* messageSendProtocol = new MessageProtocol();
	messageSendProtocol->messageHeader.command_id = id;

	messageSendProtocol->messageBody.message = message;
	messageSendProtocol->createBuffer(4);
	messageSendProtocol->sendMessage(*messageSendProtocol->buffer);
	std::vector<char> packet = messageSendProtocol->buffer->mBuffer;
	for (int i = 1; i < clientsCounter; i++)
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

//sendMessageOthersInGroup
//
//Purpouse: Send message to all other clients in the chat room exept the curent conection
//
//@param: connection Id, room name, command id, the message to be sent
//@return: void

void sendMessageOthersInGroup(int clientIndex,  std::string roomName, int id, std::string message)
{
	MessageProtocol* messageSendProtocol = new MessageProtocol();
	messageSendProtocol->messageHeader.command_id = id;

	messageSendProtocol->messageBody.message = message;
	messageSendProtocol->createBuffer(4);
	messageSendProtocol->sendMessage(*messageSendProtocol->buffer);
	std::vector<char> packet = messageSendProtocol->buffer->mBuffer;
	for (int i = 1; i < clientsCounter; i++)
	{
		if (clientIndex == i) 
		{
			continue;
		}
		if (Clients[i].room == roomName)
		{
			send(Clients[i].Connection, &packet[0], packet.size(), 0);
		}
	}
	delete messageSendProtocol;
}

void authThread(int index)
{
	char packet1[512];
	int packLength;
	bool run = true;
	while (run)
	{

		if ((packLength = recv(Clients[index].ConnectionToDS, &packet1[0], 512, NULL)) < 1) {
			closesocket(Clients[index].ConnectionToDS);
			//WSACleanup();
			run = false;
		}
		else
		{
			std::cout << "Received message from database server" << std::endl;
			ClientMessageWrapper clientMasterMessage;
			ServerMessageWrapper serverMasterMessage;
			std::string Received(packet1);
			int clientIndex;
			
			//bool success = serverMasterMessage.ParseFromString(Received);
			bool success = serverMasterMessage.ParseFromArray(packet1, strlen(packet1));
			if (!success) {
				std::cerr << "Failed to parse client master message" << std::endl;
			}
			std::cout << serverMasterMessage.DebugString() << std::endl;
			clientIndex = serverMasterMessage.clientindex();
			

			int messageID = serverMasterMessage.messageid();
			if (messageID == 1)
			{
				if (!serverMasterMessage.has_m1())
				{
					std::cerr << "Doesn't have CreateAccountWebSuccess messege" << std::endl;
					return;
				}

				CreateAccountWebSuccess caws = serverMasterMessage.m1();
				int id = caws.userid();
				std::string messageToClient = "Registration successful!; your id =" + std::to_string(id);
				Clients[clientIndex].id = id;
				sendMessageToClient(Clients[clientIndex].Connection, 0, messageToClient);
				sendMessageToClient(Clients[clientIndex].Connection, 0, "\nWhat is your name?");
				std::cout << "User was logged " << id << std::endl;


			} else if (messageID == 2)
			{
				if (!serverMasterMessage.has_m2())
				{
					std::cerr << "Doesn't have CreateAccountWebFailure messege" << std::endl;
					return;
				}

				CreateAccountWebFailure cawf = serverMasterMessage.m2();
				std::string errorMessage;
				switch (cawf.reason())
				{
				case 1:
					errorMessage = "Error: Account already exists";
					break;
				case 2:
					errorMessage = "Error: Invalid Password";
					break;
				case 0:
					errorMessage = "Error: Internal Server Error";
					break;
				}
				std::string messageToClient = errorMessage + "\n1-Login, 2-Create Account (send me a number)";
				sendMessageToClient(Clients[clientIndex].Connection, 4, messageToClient);
				std::cout << errorMessage << std::endl;

			} else if (messageID == 3)
			{
				if (!serverMasterMessage.has_m3())
				{
					std::cerr << "Doesn't have AuthenticateWebSuccess messege" << std::endl;
					return;
				}
				AuthenticateWebSuccess aws = serverMasterMessage.m3();
				std::string creattionDate = aws.creationdate();
				int userId = aws.userid();
				std::cout << "User id " << userId << " was created on "<< creattionDate << std::endl;
				std::string messageToClient = "Creation date ~  " + creattionDate;
				sendMessageToClient(Clients[clientIndex].Connection, 6, messageToClient);
				sendMessageToClient(Clients[clientIndex].Connection, 0, "\nWhat is your name?");
			}
			else if (messageID == 4)
			{
				if (!serverMasterMessage.has_m4())
				{
					std::cerr << "Doesn't have AuthenticateWebFailure messege" << std::endl;
					return;
				}
				AuthenticateWebFailure awf = serverMasterMessage.m4();
				std::string errorMessage;
				switch (awf.reason())
				{
				case 1:
					errorMessage = "Error: Invalid credentials";
					break;
				case 2:
					errorMessage = "Error: Internal Server Error";
				}
				std::string messageToClient = errorMessage + "\n1-Login, 2-Create Account (send me a number)";
				sendMessageToClient(Clients[clientIndex].Connection, 4, messageToClient);
				std::cout << errorMessage << std::endl;

			}

		}
	}
	packLength = 0;
}

void connectToDataServer(int index)
{
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
	addr.sin_port = htons(789557);
	addr.sin_family = AF_INET; //IPv4 
	
	
		Clients[index].ConnectionToDS = socket(AF_INET, SOCK_STREAM, NULL); //Creates connection socket
		if (connect(Clients[index].ConnectionToDS, (SOCKADDR*)&addr, sizeofadr) != 0)
		{
			MessageBox(NULL, "Fainled to connect", "Error", MB_OK | MB_ICONERROR);
		}
		std::cout << "Connected!" << std::endl;

		/*MessageProtocol* messageProtocol = new MessageProtocol();
		messageProtocol->createBuffer(256);*/
		
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)authThread, (LPVOID)(index), NULL, NULL);
	
}