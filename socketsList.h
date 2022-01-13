#pragma once
#include "buffer.h"
#include <winsock2.h>
#include <time.h>
#include <queue>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define MAX_SOCKETS 60
struct socketState {
	SOCKET id;
	bool empty = true;
	bool recieveStatus = false;
	bool listenStatus = false;
	bool sendStatus = false;
	Buffer buffer;
	time_t lastRequest;
	queue<Answer> answers;
};
class socketList {
private:
	int socketsCount;
	socketState *socketsArr;
public:
	socketState& operator[](int idx) { return socketsArr[idx]; }
	const socketState& operator[](int idx) const {
		return socketsArr[idx];
	}

	socketList() {
		socketsArr = new socketState[MAX_SOCKETS];
		socketsCount = 0;
	}

	~socketList() {
		delete [] socketsArr;
	}

	bool addSocket(SOCKET id)
	{
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if (socketsArr[i].empty)
			{
				socketsArr[i].id = id;
				socketsArr[i].recieveStatus = true;
				socketsArr[i].sendStatus = false;
				socketsArr[i].empty = false;
				socketsCount++;
				updateSocket(i);
				return (true);
			}
		}
		return (false);
	}

	void removeSocket(int index) {
		socketsArr[index].empty = true;
		socketsArr[index].recieveStatus = false;
		socketsArr[index].sendStatus = false;
		socketsCount--;
	}

	void updateSocket(int index) {
		socketsArr[index].lastRequest = time(0);
	}

	bool checkConnection(int index) {
		return ((time(0) - socketsArr[index].lastRequest) < 180);
	}

	void pushAnswer(Answer answer,int index) {
		socketsArr[index].answers.push(answer);
	}

	Answer popAnswer(int index) {
		Answer res= socketsArr[index].answers.front();
		socketsArr[index].answers.pop();
		return res;
	}

};