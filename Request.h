#pragma once
#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include "Answer.h"
#include <stdio.h>
#include <ios>
using namespace std;
void printArr(const char* str) {//debug
	int i = 0;
	while (str[i] != '\0') {
		cout << '[' << i << ']' << str[i] << endl;
		i++;
	}
}

class noParamsException : public exception {};

enum RequestType { OPTIONS, GET, HEAD, POST, PUT, DEL, TRACE };
enum Lang { he, en, fr };

class Request {
	RequestType type;
	Lang lang;
	char body[256];
	char fileName[64];
	Answer answer;


	int parseRequestType(char* buffer) {
		int point = 0;
		int end;
		char request[10];

		while (buffer[point] != ' ') {
			point++;
		}
		end = point;
		memcpy(request, buffer, end);
		request[end] = '\0';


		if (!strcmp(request, "OPTIONS")) {
			type = OPTIONS;
		}
		else if (!strcmp(request, "GET")) {
			type = GET;
		}
		else if (!strcmp(request, "HEAD")) {
			type = HEAD;
		}
		else if (!strcmp(request, "POST")) {
			type = POST;
		}
		else if (!strcmp(request, "PUT")) {
			type = PUT;
		}
		else if (!strcmp(request, "DELETE")) {
			type = DEL;
		}
		else if (!strcmp(request, "TRACE")) {
			type = TRACE;
		}
		return end;
	}

	int parseFileName(char* buffer, int point) {
		while (buffer[point] != '/') {
			point++;
		}
		point++;
		int end = point;
		while ((buffer[end]) != ' ') {
			end++;
		}
		memcpy(fileName, buffer + point, end - point);
		fileName[end - point] = '\0';
		return end;
	}

	int parseParams(char* buffer, int point) {
		int end;
		char key[10];

		while (buffer[point] != '/') {
			point++;
		}
		point++;
		if (buffer[point] != '?') {
			throw noParamsException();
		}
		point++;
		end = point;
		while (buffer[end] != '=') {
			end++;
		}
		memcpy(key, buffer + point, end);
		key[end - point] = '\0';


		if (!strcmp(key, "lang")) {
			return parseLangValue(buffer, end);
		}
		return end;
	}

	int parseLangValue(char* buffer, int point) {
		char value[3];
		point++;
		memcpy(value, buffer + point, 2);
		value[2] = '\0';

		if (!strcmp(value, "he")) {
			lang = he;
		}
		else if (!strcmp(value, "en")) {
			lang = en;
		}
		else if (!strcmp(value, "fr")) {
			lang = fr;
		}
		return point + 2;
	}

	void readBody(char* buffer) {
		int contentLength = getContentLength(buffer);
		if (!contentLength) {
			body[0] = '\0';
			return;
		}
		int begin = getBodyBegin(buffer);
		memcpy(body, buffer + begin, contentLength);
	}

	int getBodyBegin(char* buffer) {
		int p = 0;
		while ((buffer[p - 4] != '\r') || (buffer[p - 3] != '\n') || (buffer[p - 2] != '\r') || (buffer[p - 1] != '\n')) {
			p++;
		}
		return p;
	}

	bool number(char ch) {
		return ((ch <= '9') && (ch >= '0'));
	}

	int getContentLength(char* buffer) {
		char str[15];
		char res[10];

		for (int p = 0; p < 512; p++) {
			if (buffer[p] == 'C') {
				memcpy(str, buffer + p, 14);
				str[14] = '\0';
				if (!strcmp(str, "Content-Length")) {
					p += 16;
					int end = p;

					while (number(buffer[end])) {
						end++;
					}

					memcpy(res, buffer + p, end - p);
					res[end - p] = '\0';
					return atoi(res);
				}
			}
		}
		return -1;
	}

	//request

	void options() {
		answer.setOptionType();
	}

	void get(char *buffer,int point) {
		try {
			parseParams(buffer, point);
		}
		catch (noParamsException& e) {
			lang = en;
		}

		if (lang == en) {
			ifstream f("en.html");
				answer.setBody(f);
		}
		if (lang == he) {
			ifstream f("he.html");
			answer.setBody(f);
		}
		if (lang == fr) {
			ifstream f("fr.html");
			answer.setBody(f);
		}
	}

	void post(char *buffer) {
		readBody(buffer);
		cout << body;
	}

	void put(char *buffer,int point) {
		parseFileName(buffer,point);
		
		if (!strlen(fileName)) {
			answer.setStatus("501 Not Implemented");
			return;
		}

		ofstream f(fileName);

		if (!f) {
			answer.setStatus("501 Not Implemented");
			return;
		}
		readBody(buffer);
		f.write(body,strlen(body));
	}

	void del(char* buffer, int point) {
		parseFileName(buffer, point);

		if (!strlen(fileName)) {
			answer.setStatus("501 Not Implemented");
			return;
		}

		
		if ((remove(fileName))) {
			answer.setStatus("501 Not Implemented");
			return;
		}

	}

	void trace(char* buffer, int point) {
		parseFileName(buffer, point);
		char* fileBody;
		if (!strlen(fileName)) {
			answer.setStatus("501 Not Implemented");
			return;
		}

		ifstream f(fileName); 
		
		if (!f) {
			answer.setStatus("501 Not Implemented");
			return;
		}
		answer.setBody(f);
	}

	char* getFileName() {
		return fileName;
	}
	

public:
	Request(char* buffer) {
		fileName[0] = '\0';
		    int point;
			point = parseRequestType(buffer);
			
			switch (type) {
			case OPTIONS:
				options();
				break;
			case GET:
				get(buffer,point);
				break;
			case HEAD:
				get(buffer, point);
				break;
			case POST:
				post(buffer);
				break;
			case PUT:
				put(buffer,point);
				break;
			case DEL:
				del(buffer,point);
				break;
			case TRACE:
				trace(buffer,point);
				break;
			}
	}

	Answer getAnswer() {
		return answer;
	}



};

