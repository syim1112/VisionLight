#pragma once
#include <windows.h>
#include <thread>
#include <iostream>

class CSerialComm {
public:
    CSerialComm();
    ~CSerialComm();

    BOOL OpenPort(CString portName, DWORD nBaudRate, DWORD nByteSize);
    void ClosePort();
    BOOL WriteData(const char* data, DWORD size);
    void StartAsyncRead();
    void StopAsyncRead();
    void SetReadCallback(void (*callback)(const char*, DWORD));

private:
    HANDLE m_hComm;         // 시리얼 포트 핸들
    BOOL m_bOpened;         // 포트가 열렸는지 상태
    OVERLAPPED m_osWrite;   // 쓰기 작업용 OVERLAPPED 구조체
    OVERLAPPED m_osRead;    // 읽기 작업용 OVERLAPPED 구조체
    std::thread m_readThread; // 읽기 스레드
    BOOL m_stopRead;        // 읽기 중지 플래그
    void (*m_readCallback)(const char*, DWORD); // 읽기 콜백 함수 포인터

    void ReadThreadFunction(); // 읽기 스레드 함수
};
