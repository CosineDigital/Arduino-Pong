#pragma once

#include "Serial.hpp"

SerialPort::SerialPort() : open(true) {

    handle = CreateFileA(
        "\\\\.\\COM3",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    std::cout << handle << std::endl;

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

    dcbSerialParam.BaudRate = CBR_9600;
    dcbSerialParam.ByteSize = 8;
    dcbSerialParam.StopBits = ONESTOPBIT;
    dcbSerialParam.Parity = NOPARITY;

    if (!SetCommState(handle, &dcbSerialParam)) {
        issueError();

        open = false;
    }

    COMMTIMEOUTS timeout = { 0 };
    timeout.ReadIntervalTimeout = 60;
    timeout.ReadTotalTimeoutConstant = 60;
    timeout.ReadTotalTimeoutMultiplier = 15;
    timeout.WriteTotalTimeoutConstant = 60;
    timeout.WriteTotalTimeoutMultiplier = 8;
    if (!SetCommTimeouts(handle, &timeout)) {
        issueError();

        open = false;
    }
}

int SerialPort::readBytes(char* msg, int length) const noexcept {

    DWORD dwRead = 0;
    if (!ReadFile(handle, msg, length, &dwRead, NULL)) {
        std::cout << dwRead << std::endl;
        issueError();
        return 1;
    }

    std::cout << dwRead << std::endl;

    return 0;
}

int SerialPort::writeBytes(char* msg, int length) const noexcept {

    DWORD dwRead = 0;
    if (!WriteFile(handle, msg, length, &dwRead, NULL)) {

        issueError();
        return 1;
    }

    return 0;
}

void SerialPort::issueError(void) const noexcept {
    wchar_t lastErr[1020];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        lastErr,
        1020,
        NULL);
}

SerialPort::~SerialPort() {
    CloseHandle(handle);
}