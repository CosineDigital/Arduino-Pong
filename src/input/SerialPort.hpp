#pragma once
#include <Windows.h>
#include <iostream>

class SerialPort {
		
private:
	HANDLE handle;

public:
	bool open;

    SerialPort() {}

    SerialPort(const char* portName) : open(true) {

        handle = CreateFileA(
            portName,
            GENERIC_READ,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_READONLY,
            NULL);

        if (handle == INVALID_HANDLE_VALUE) {
            if (GetLastError() == ERROR_FILE_NOT_FOUND) {
                std::cout << "Port not found\n";
                issueError();
            }

            std::cout << "Invalid handle\n";
            issueError();

            open = false;
        }

        DCB dcbSerialParam = { 0 };
        dcbSerialParam.DCBlength = sizeof(dcbSerialParam);

        if (!GetCommState(handle, &dcbSerialParam)) {
            issueError();

            open = false;
        }

        dcbSerialParam.BaudRate = 9600;
        dcbSerialParam.ByteSize = 8;
        dcbSerialParam.StopBits = ONESTOPBIT;
        dcbSerialParam.Parity = NOPARITY;

        if (!SetCommState(handle, &dcbSerialParam)) {
            issueError();

            open = false;
        }

        COMMTIMEOUTS timeout = { 0 };
        timeout.ReadIntervalTimeout = 50; //60
        timeout.ReadTotalTimeoutConstant = 50; //60
        timeout.ReadTotalTimeoutMultiplier = 50;
        timeout.WriteTotalTimeoutConstant = 50;
        timeout.WriteTotalTimeoutMultiplier = 10;
        if (!SetCommTimeouts(handle, &timeout)) {
            issueError();

            open = false;
        }
    }

    int readBytes(char* msg, int length) const noexcept {

        DWORD dwRead = 0;
        if (!ReadFile(handle, msg, length, &dwRead, NULL)) {

            std::cout << *msg << '\n';
            std::cout << dwRead << '\n';

            issueError();
            return 1;
        }

        return 0;
    }

    int writeBytes(char* msg, int length) const noexcept {

        DWORD dwRead = 0;
        if (!WriteFile(handle, msg, length, &dwRead, NULL)) {

            issueError();
            return 1;
        }

        return 0;
    }

    int readByte(char* byte) const noexcept {

        DWORD dwRead = 0;
        if (!ReadFile(handle, byte, 1, &dwRead, NULL)) {
            issueError();

            return 1;
        }

        return 0;
    }

    void issueError(void) const noexcept {

        wchar_t lastErr[1020];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            lastErr,
            1020,
            NULL);

        std::cout << lastErr << std::endl;
    }

    ~SerialPort() {
        CloseHandle(handle);
    }
};
