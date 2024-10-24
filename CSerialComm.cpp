#include "pch.h"
#include "CSerialComm.h"
#include <iostream>

CSerialComm::CSerialComm() : m_hComm(INVALID_HANDLE_VALUE), m_bOpened(FALSE), m_stopRead(FALSE), m_readCallback(nullptr) {
    ZeroMemory(&m_osWrite, sizeof(OVERLAPPED));
    ZeroMemory(&m_osRead, sizeof(OVERLAPPED));
}

CSerialComm::~CSerialComm() {
    ClosePort();
}

BOOL CSerialComm::OpenPort(CString portName, DWORD nBaudRate, DWORD nByteSize) {
    m_hComm = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (m_hComm == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    DCB dcb;
    GetCommState(m_hComm, &dcb);

    dcb.BaudRate = nBaudRate;
    dcb.ByteSize = nByteSize;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;

    if (!SetCommState(m_hComm, &dcb)) {
        ClosePort();
        return FALSE;
    }

    // 타임아웃 설정
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    SetCommTimeouts(m_hComm, &timeouts);

    m_bOpened = TRUE;
    return TRUE;
}

void CSerialComm::ClosePort() {
    if (m_bOpened && m_hComm != INVALID_HANDLE_VALUE) {
        m_stopRead = TRUE;
        if (m_readThread.joinable()) {
            m_readThread.join();
        }
        CloseHandle(m_hComm);
        m_hComm = INVALID_HANDLE_VALUE;
        m_bOpened = FALSE;
    }
}

BOOL CSerialComm::WriteData(const char* data, DWORD size) {
    if (!m_bOpened) return FALSE;

    DWORD bytesWritten = NULL;
    if (!WriteFile(m_hComm, data, size, &bytesWritten, &m_osWrite)) {
        if (GetLastError() == ERROR_IO_PENDING) {
            // 쓰기 완료 대기
            if (!GetOverlappedResult(m_hComm, &m_osWrite, &bytesWritten, TRUE)) {
                return FALSE;
            }
        }
        else {
            return FALSE;
        }
    }
    else {
        std::cout << data << std::endl;
    }

    return TRUE;
}

void CSerialComm::StartAsyncRead() {
    m_stopRead = FALSE;
    m_readThread = std::thread(&CSerialComm::ReadThreadFunction, this);
}

void CSerialComm::StopAsyncRead() {
    m_stopRead = TRUE;
    if (m_readThread.joinable()) {
        m_readThread.join();
    }
}

void CSerialComm::SetReadCallback(void (*callback)(const char*, DWORD)) {
    m_readCallback = callback;
}

void CSerialComm::ReadThreadFunction() {
    char buffer[100];
    DWORD bytesRead;

    while (!m_stopRead) {
        if (!ReadFile(m_hComm, buffer, sizeof(buffer), &bytesRead, &m_osRead)) {
            if (GetLastError() == ERROR_IO_PENDING) {
                // 읽기 완료 대기
                if (GetOverlappedResult(m_hComm, &m_osRead, &bytesRead, TRUE)) {
                    if (m_readCallback && bytesRead > 0) {
                        m_readCallback(buffer, bytesRead);
                    }
                }
            }
        }
        else {
            if (bytesRead > 0) {
                if (m_readCallback) {
                    m_readCallback(buffer, bytesRead);
                }
            }
        }
    }
}
