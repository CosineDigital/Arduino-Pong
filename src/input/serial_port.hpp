#pragma once
#include <Windows.h>
#include <iostream>
#include <chrono>
#include <thread>

class SerialPort {
		
public:

    SerialPort() : handle(0) {}

    int init(const char* portName) {

        handle = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

        if (handle == INVALID_HANDLE_VALUE) {
            if (GetLastError() == ERROR_FILE_NOT_FOUND) {
                std::cerr << "Port " << portName << " not found\n";
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
                    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                }
            }
        }

        // success
        return 1;
    }

    int readByte(unsigned char* data) const noexcept {

        DWORD bytesRead = 0;
        unsigned int toRead = 0;

        DWORD errors;
        COMSTAT status;

        ClearCommError(handle, &errors, &status);

        if (status.cbInQue > 0)
        {
            if (status.cbInQue > 1)
            {
                toRead = 1;
            }
            else
            {
                toRead = status.cbInQue;
            }
        }

        return ReadFile(handle, (void*)data, toRead, &bytesRead, NULL);
    }

    inline int writeByte(char data) const noexcept {
        DWORD bytesSend;

        return WriteFile(handle, (void*)&data, 1, &bytesSend, 0);
    }

    ~SerialPort() {
        CloseHandle(handle);
    }
private:
    HANDLE handle;

};
