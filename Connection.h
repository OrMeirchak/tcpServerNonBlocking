#pragma once
#include <string.h>
#include "Request.h"
#include "ConnectionException.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "socketsList.h"
#define _CRT_SECURE_NO_WARNINGS
class Connection {
public:


private:

	WSAData WSA;
	socketState listenSocket;
	sockaddr_in service;
	socketList sockets;
	int PORT;


	// Send and receive data.
	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];

	void initializeWindowsSocket() {
		if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &WSA)) {
			throw wsaStartupException();
		}
	}

	void createListenSocket() {
		listenSocket.id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		listenSocket.empty = false;
	
		if (INVALID_SOCKET == listenSocket.id) {
			throw socketException();
		}
	}

	void createServerSocket() {
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = INADDR_ANY;
		service.sin_port = htons(PORT);
	}

	void bindConnectionToSocket() {
		if (SOCKET_ERROR == bind(listenSocket.id, (SOCKADDR*)&service, sizeof(service))) {
			throw bindException(listenSocket.id);
		}
	}

	void acceptListen() {
		if (SOCKET_ERROR == listen(listenSocket.id, 5))
		{
			cout << "Time Server: Error at listen(): " << WSAGetLastError() << endl;
			closesocket(listenSocket.id);
			WSACleanup();
			return;
		}
		listenSocket.listenStatus = true;
	}

public:
	Connection(const int port) {

		PORT = port;

		try {
			initializeWindowsSocket();
			createListenSocket();
			createServerSocket();
			bindConnectionToSocket();
			acceptListen();
		}
		catch (ConnectionException& e) {
			cout << e.msg();
		}
	}

	~Connection() {
		closesocket(listenSocket.id);
		WSACleanup();
		cout << "Closing Connection.\n";
	}

	socketList& getSocketsList() {
		return sockets;
	}

	SOCKET& getListenSocket() {
		return listenSocket.id;
	}

	void sendTo(const char* sendBuff) {
		bytesSent = sendto(listenSocket.id, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
		if (SOCKET_ERROR == bytesSent) {
			throw sentToException(listenSocket.id);
		}
	}

	char* recieveFrom() {
		bytesRecv = recvfrom(listenSocket.id, recvBuff, 255, 0, &client_addr, &client_addr_len);

		if (SOCKET_ERROR == bytesRecv) {
			throw recieveException(listenSocket.id);
		}
		recvBuff[bytesRecv] = '\0';
		return recvBuff;
	}

	void acceptConnection(){
		SOCKET id = getListenSocket();
		struct sockaddr_in from;		
		int fromLen = sizeof(from);

		SOCKET msgSocket = accept(id, (struct sockaddr*)&from, &fromLen);
		if (INVALID_SOCKET == msgSocket)
		{
			cout << "Error at accept(): " << WSAGetLastError() << endl;
			return;
		}
		cout << "Client " << ":" << ntohs(from.sin_port) << " is connected." << endl;

		
		unsigned long flag = 1;
		if (ioctlsocket(msgSocket, FIONBIO, &flag) != 0)
		{
			cout << "Error at ioctlsocket(): " << WSAGetLastError() << endl;
		}

		if (sockets.addSocket(msgSocket) == false)
		{
			cout << "\t\tToo many connections, dropped!\n";
			closesocket(id);
		}
		return;
	}

	void printArr(const char* str) {//debug
		int i = 0;
		while ((str[i] != '\0')&&(i<128)) {
			cout << '[' << i << ']' << str[i] << endl;
			i++;
		}
	}

	void receiveMessage(int index){
		SOCKET msgSocket = sockets[index].id;
		char buffer[512];

		int bytesRecv = recv(msgSocket,buffer,512, 0);

		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Error at recv(): " << WSAGetLastError() << endl;
			closesocket(msgSocket);
			sockets.removeSocket(index);
			return;
		}
		if (bytesRecv == 0)
		{
			closesocket(msgSocket);
			sockets.removeSocket(index);
			return;
		}
		
		sockets.updateSocket(index);
		Request request(buffer);
		sockets.pushAnswer(request.getAnswer(),index);
		sockets[index].sendStatus = true;
	}

	void sendMessage(int index)
	{
		int bytesSent = 0;

		SOCKET msgSocket = sockets[index].id;

		char buffer[1000];

		sprintf(buffer, "%s", sockets.popAnswer(index).getMsg());

		bytesSent = send(msgSocket, buffer, (int)strlen(buffer), 0);
		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Error at send(): " << WSAGetLastError() << endl;
			return;
		}

		sockets[index].sendStatus = false;
	}
};

