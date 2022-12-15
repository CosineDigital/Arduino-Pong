#pragma once
#include <Windows.h>
#include <iostream>

class SerialPort {
		
private:
	HANDLE handle;

public:
	bool open;

    SerialPort() : handle(0), open(false) {}

    int init(const char* portName) {

        handle = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if (handle == INVALID_HANDLE_VALUE) {
            if (GetLastError() == ERROR_FILE_NOT_FOUND) {
                std::cerr << "Port not found\n";
                return 0;
            }
            else {
                std::cerr << "Could not create handle\n";
                return 0;
            }
        }
        else {
            DCB dcbSerialParam = { 0 };

            if (!GetCommState(handle, &dcbSerialParam)) {
                std::cerr << "Could not get comm state\n";
                return 0;
            }
            else {
                dcbSerialParam.BaudRate = CBR_9600;
                dcbSerialParam.ByteSize = 8;
                dcbSerialParam.StopBits = ONESTOPBIT;
                dcbSerialParam.Parity = NOPARITY;
                dcbSerialParam.fDtrControl = DTR_CONTROL_ENABLE;

                if (!SetCommState(handle, &dcbSerialParam)) {
                    std::cerr << "Could not set comm state";
                    return 0;
                }
                else {
                    PurgeComm(handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
                }

                COMMTIMEOUTS timeout = { 0 };
                timeout.ReadIntervalTimeout = 60;
                timeout.ReadTotalTimeoutConstant = 60;
                timeout.ReadTotalTimeoutMultiplier = 15;
                timeout.WriteTotalTimeoutConstant = 60;
                timeout.WriteTotalTimeoutMultiplier = 8;

                if (!SetCommTimeouts(handle, &timeout)) {
                    std::cerr << "Could not set comm timeouts\n";
                    return 0;
                }
            }
        }

        // success
        return 1;
    }

    int readBytes(char* msg, int length) const noexcept {
        
        /*
        COMSTAT status;

        DWORD last_error;
        DWORD dwRead = 0;
        auto toRead = 0;

        ClearCommError(handle, &last_error, &status);
        
        if (!ReadFile(handle, msg, length, &dwRead, NULL)) {
            std::cout << msg << std::endl;
        }

        if (status.cbInQue > 0) {
            if (status.cbInQue > length) {
                toRead = length;
            }
            else 
                toRead = status.cbInQue;

            if (!ReadFile(handle, msg, toRead, &dwRead, NULL)) {
                std::cout << dwRead << std::endl;
                return 0;
            }
            else {
                std::cout << dwRead << std::endl;
                issueError();
            }
        }
        else {
            std::cout << "nothing in queue\n";
        }

        if (ReadFile(handle, msg, toRead, &dwRead, NULL)) {
            std::cout << dwRead << std::endl;
            return 0;
        }
        */
        
        DWORD dwRead = 0;
        if (!ReadFile(handle, msg, length, &dwRead, NULL)) {
            return 1;
        }
        else {
            std::cerr << "Could not read bytes\n";
            return 0;
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
        if (ReadFile(handle, byte, 1, &dwRead, NULL)) {
            return 1;
        }
        else {
            std::cerr << "Could not read byte\n";
            return 0;
        }
    }

    void issueError(void) const noexcept {

        std::cout << GetLastError() << std::endl;

        wchar_t lastErr[1020];
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            lastErr,
            1020,
            NULL);
    }

    ~SerialPort() {
        CloseHandle(handle);
    }
};
