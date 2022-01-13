#pragma once
#include <iostream>


class Answer {
private:
 
char body[256];
	char msg[512];
	char status[64];
	const char* SERVER_NAME = "myServer";
	bool optionsTypeRequest;

	void setMsg() {
		if (optionsTypeRequest) {
			sprintf(msg, "HTTP/1.1 %s\r\nserver: %s\r\nContent-Length: 0\r\nAllow: OPTIONS,GET,HEAD,POST,PUT,DELETE,TRACE\r\n\r\n", status, SERVER_NAME);
		}
		else {
			sprintf(msg, "HTTP/1.1 %s\r\nserver: %s,\r\nContent-Length: %d\r\n\r\n%s", status, SERVER_NAME, strlen(body), body);
		}
	}
public:
	Answer() {
		optionsTypeRequest = false;
		strcpy(status, "200 OK");
		body[0] = '\0';
	}

	Answer(const Answer& other):optionsTypeRequest(other.optionsTypeRequest){
		memcpy(body, other.body, strlen(other.body)+1);
		memcpy(msg, other.msg, strlen(other.msg)+1);
		memcpy(status, other.status, strlen(other.status)+1);
	}

	void setStatus(const char* status) {
		sprintf(this->status,"%s", status);
	}

	void setBody(const char* body) {
		sprintf(this->body,"%s", body);
	}

	void setBody(ifstream &f) {
		f.read(body,256);
	}

	void setOptionType() {
		optionsTypeRequest = true;
	}

	char* getMsg() {
		setMsg();
		return msg;
	}

};

