#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <exception>
#include <iostream>
#include <string>
using namespace std;

class ConnectionException : public exception {
public:
	virtual string msg() const throw() = 0;
};

class wsaStartupException : public ConnectionException {
public:
	string msg() const throw() {
		return "Time Client: Error at WSA Startup\n";
	}
};

class socketException : public ConnectionException {
public:
	string msg() const throw() {
		string res = "Time Client: Error at socket : " + to_string(WSAGetLastError());
		WSACleanup();
		return res;
	}
};

class bindException : public ConnectionException {
public:
	SOCKET& Socket;

	bindException(SOCKET& Socket) :Socket(Socket) {};


	string msg() const throw() {
		string res = "Time Server: Error at bind : " + to_string(WSAGetLastError());
		closesocket(Socket);
		WSACleanup();
		return res;
	}
};

class sentToException : public ConnectionException {
public:
	SOCKET& Socket;

	sentToException(SOCKET& Socket) :Socket(Socket) {};

	string msg() const throw() {
		string res = "Server: Error at sendto : " + to_string(WSAGetLastError());
		closesocket(Socket);
		WSACleanup();
		return res;
	}
};

class recieveException : public ConnectionException {
public:
	SOCKET& Socket;

	recieveException(SOCKET& Socket) :Socket(Socket) {};

	string msg() const throw() {

		string res = "Time Server: Error at recvfrom : " + to_string(WSAGetLastError());
		closesocket(Socket);
		WSACleanup();
		return res;
	}
};

