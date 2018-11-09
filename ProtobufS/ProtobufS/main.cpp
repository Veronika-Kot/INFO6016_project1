#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>
#include <stdio.h>
#include <time.h> 
#include "../gen/chat.pb.h"
#include "DBconnector.h"

SOCKET Connections[100];
int clientsCounter = 1;
int iResult;

DBconnector dbHelper;
bool addNewClient(std::string email, std::string password, int& id);
bool authenticateTheClient(std::string email, std::string password, int id, std::string& creationDateStr);
void connectToDB();
bool checkIfAccountExists(std::string email, int& id);

void sendMessageToClient(SOCKET theConnection, char* buffer);

void handleClients(int index)
{
	std::cout << "Cleint index " << index<<std::endl;
	int packLength;
	char packet1[512];
	
	bool run = true;
	while (run)
	{
		//std::vector<char> packet(512);
		int clientIndex;
		packLength = recv(Connections[index], &packet1[0], 512, NULL);
		if (packLength == 0) {
			closesocket(Connections[index]);
			WSACleanup();
			std::cout << "Buffer length is less then 1" << std::endl;
			run = false;
		}
		else if(packLength > 0)
		{
			std::cout << "Received message from chat server" << std::endl;
			ClientMessageWrapper clientMasterMessage;
			std::string receivedMessage(packet1);
			bool success = clientMasterMessage.ParseFromString(receivedMessage);
			if (!success) {
				std::cerr << "Failed to parse client master message" << std::endl;
				continue;
			}

			std::cout << clientMasterMessage.DebugString() << std::endl;
			
			clientIndex = clientMasterMessage.clientindex();
			int messageID = clientMasterMessage.messageid();
			if (messageID == 1)
			{
				if (!clientMasterMessage.has_m1())
				{
					std::cerr << "Doesn't have CreateAccountWeb message" << std::endl;
					continue;
				}

				CreateAccountWeb caw = clientMasterMessage.m1();
				std::string email = caw.email();
				std::string password = caw.plaintextpassword();
				int id;
				
				if (checkIfAccountExists(email, id))
				{
					std::cout << "Check for account exists" << std::endl;
					ServerMessageWrapper serverMasterMessage;
					serverMasterMessage.set_clientindex(clientIndex);
					CreateAccountWebFailure* awf = new CreateAccountWebFailure();
					awf->set_reason(1);
					serverMasterMessage.set_allocated_m2(awf);
					serverMasterMessage.set_messageid(2);

					int size = serverMasterMessage.ByteSize();
					char * buffer = new char[size];
					serverMasterMessage.SerializeToArray(buffer, size);
					sendMessageToClient(Connections[index], buffer);
					continue;
				
				} else if (password.length() < 3)
				{
					std::cout << "check for password length" << std::endl;
					ServerMessageWrapper serverMasterMessage;
					serverMasterMessage.set_clientindex(clientIndex);
					CreateAccountWebFailure* awf = new CreateAccountWebFailure();
					awf->set_reason(2);
					serverMasterMessage.set_allocated_m2(awf);
					serverMasterMessage.set_messageid(2);

					int size = serverMasterMessage.ByteSize();
					char * buffer = new char[size];
					serverMasterMessage.SerializeToArray(buffer, size);
					sendMessageToClient(Connections[index], buffer);
					continue;

				} else if (addNewClient(email, password, id))
				{
					std::cout << "Added new client" << std::endl;
					ServerMessageWrapper serverMasterMessage;
					serverMasterMessage.set_clientindex(clientIndex);
					CreateAccountWebSuccess* aws = new CreateAccountWebSuccess();
					aws->set_userid(id);
					serverMasterMessage.set_allocated_m1(aws);
					serverMasterMessage.set_messageid(1);
					int size = serverMasterMessage.ByteSize();
					char * buffer = new char[size];
					serverMasterMessage.SerializeToArray(buffer, size);
					sendMessageToClient(Connections[index], buffer);
					continue;

				}
				else {
					std::cout << "Else if 1" << std::endl;
					ServerMessageWrapper serverMasterMessage;
					serverMasterMessage.set_clientindex(clientIndex);
					CreateAccountWebFailure* awf = new CreateAccountWebFailure();
					serverMasterMessage.set_allocated_m2(awf);
					awf->set_reason(0);
					serverMasterMessage.set_messageid(2);
					int size = serverMasterMessage.ByteSize();
					char * buffer = new char[size];
					serverMasterMessage.SerializeToArray(buffer, size);
					sendMessageToClient(Connections[index], buffer);
					continue;
				}

			}
			else if (messageID == 2)
			{
				if (!clientMasterMessage.has_m2())
				{
					std::cerr << "Doesn't have AuthenticateWeb messega" << std::endl;
					continue;
				}

				AuthenticateWeb aw = clientMasterMessage.m2();
				std::string email = aw.email();
				std::string password = aw.plaintextpassword();
				int userID;
				std::string creationDate;

				if (!checkIfAccountExists(email, userID))
				{
					std::cout << "Check id account doesn;t exists" << std::endl;
					ServerMessageWrapper serverMasterMessage;
					serverMasterMessage.set_clientindex(clientIndex);
					AuthenticateWebFailure* awf = new AuthenticateWebFailure();
					awf->set_reason(1);
					serverMasterMessage.set_allocated_m4(awf);
					serverMasterMessage.set_messageid(4);
					int size = serverMasterMessage.ByteSize();
					char * buffer = new char[size];
					serverMasterMessage.SerializeToArray(buffer, size);
					sendMessageToClient(Connections[index], buffer);
					continue;
				}
				else if (!authenticateTheClient(email, password, userID, creationDate))
				{
					std::cout << "Check if password correct" << std::endl;
					ServerMessageWrapper serverMasterMessage;
					serverMasterMessage.set_clientindex(clientIndex);
					AuthenticateWebFailure* awf = new AuthenticateWebFailure();
					awf->set_reason(1);
					serverMasterMessage.set_allocated_m4(awf);

					serverMasterMessage.set_messageid(4);
					std::string message;
					
					int size = serverMasterMessage.ByteSize();
					char * buffer = new char[size];
					serverMasterMessage.SerializeToArray(buffer, size);
					sendMessageToClient(Connections[index], buffer);
					continue;
				}
				else{
					std::cout << "login user" << std::endl;
					ServerMessageWrapper serverMasterMessage;
					serverMasterMessage.set_clientindex(clientIndex);
					AuthenticateWebSuccess* aws = new AuthenticateWebSuccess();
					aws->set_creationdate(creationDate);
					aws->set_userid(userID);
					serverMasterMessage.set_allocated_m3(aws);
					serverMasterMessage.set_messageid(3);
					int size = serverMasterMessage.ByteSize();
					char * buffer = new char[size];
					serverMasterMessage.SerializeToArray(buffer, size);
					sendMessageToClient(Connections[index], buffer);
					continue;
				}
			}
		} 
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(Connections[index]);
			WSACleanup();
			return;
		}
	}
	packLength = 0;
}





int main()
{
	connectToDB();	
	
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
	addr.sin_port = htons(789557);
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

			Connections[i] = newConnection;
			clientsCounter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)handleClients, (LPVOID)(i), NULL, NULL); //Create a thread
		}
	}
	


	system("pause");
	return 0;
}


void sendMessageToClient(SOCKET theConnection, char* buffer)
{
	//char bufferServerMessage[512];
	//strcpy(bufferServerMessage, stringBuff.c_str());
	//std::cout << bufferServerMessage << std::endl;
	send(theConnection, &buffer[0], (int)strlen(buffer), 0);

}
