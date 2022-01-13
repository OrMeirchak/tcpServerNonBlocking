#pragma once
class Buffer {
private:
	char buff[128];
	int len;

public:
	Buffer() {
		len = 0;
	}

	char& operator[](int idx) { return buff[idx]; }
	const char& operator[](int idx) const {
		return buff[idx];
	}


	int getLen() {
		return len;
	}

	int getSize() {
		return sizeof(buff);
	}

	char* getBuffer() {
		return buff;
	}
};
