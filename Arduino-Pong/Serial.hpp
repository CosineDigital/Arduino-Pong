#pragma once
#include <Windows.h>
#include <iostream>

class SerialPort {
		
private:
	HANDLE handle;

public:
	explicit SerialPort();
	~SerialPort();

	int writeBytes(char*, int) const noexcept;
	int readBytes(char*, int) const noexcept;

	void issueError(void) const noexcept;

	bool open;
};
