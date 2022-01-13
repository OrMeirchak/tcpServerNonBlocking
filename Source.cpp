#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include "Connection.h"
#include "socketsList.h"
#include <filesystem>

#define PORT	8080

void main()
{
	Connection connection(PORT);
	socketList& socketsList = connection.getSocketsList();

	int i = 0;
	while (true)
	{
		fd_set waitRecv;
		FD_ZERO(&waitRecv);
		FD_SET(connection.getListenSocket(), &waitRecv);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if (!socketsList.checkConnection(i)) {
				socketsList.removeSocket(i);
			}
			if (socketsList[i].recieveStatus) {
				FD_SET(socketsList[i].id, &waitRecv);
			}
		}

		fd_set waitSend;
		FD_ZERO(&waitSend);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if (socketsList[i].sendStatus)
				FD_SET(socketsList[i].id, &waitSend);
		}

		int nfd;
		nfd = select(0, &waitRecv, &waitSend, NULL, NULL);
		if (nfd == SOCKET_ERROR){
			cout << "Error at select(): " << WSAGetLastError() << endl;
			WSACleanup();
			return;
		}

		if (FD_ISSET(connection.getListenSocket(), &waitRecv)) {
			connection.acceptConnection();
		}

	

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++){
			if (FD_ISSET(socketsList[i].id, &waitRecv))
			{
				nfd--;
				if (socketsList[i].recieveStatus) {
					connection.receiveMessage(i);
				}
			}
		}

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(socketsList[i].id, &waitSend)){
				connection.sendMessage(i);	
				}
			}
		
	}
}