
// VisionLightTestDlg.h: 헤더 파일
//

#pragma once
#include <iostream>
#include <thread>
#include <functional>
#include "CSerialComm.h"
#include <vector>

using CallbackFunc = std::function<void(const wchar_t*, DWORD)>;

// CVisionLightTestDlg 대화 상자
class CVisionLightTestDlg : public CDialogEx
{
// 생성입니다.
public:
	CVisionLightTestDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	void SetCallback(CallbackFunc cb);
	HANDLE OpenComPort(CString portName, int nBaudRate, int nByteSize);
	void CloseComPort(HANDLE* hSerial);
	void ReadFromPort(HANDLE hSerial);

	void WriteToPort(HANDLE hSerial, CString data);
	CallbackFunc callback_;
	std::thread readingThread_;
	bool keepReading_;

	CSerialComm m_serialComm;

	void InitComboBox();

	HANDLE hSerialLight;
	CComboBox m_port;
	CComboBox m_bit_second;
	CComboBox m_data_bit;
	BOOL m_default_set;

	CString m_str_port;
	int m_nbit_second;
	int m_ndata_bit;
	CString m_write_data;
	CString m_frame_repetition;
	CString m_frame_millisecond;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VISIONLIGHTTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedButtonWrite();
	afx_msg void OnBnClickedCheckDefaultSet();
	afx_msg void OnBnClickedButtonFrameRepetitionSet();
	afx_msg void OnBnClickedButtonFrameMillisecondSet();
	afx_msg void OnBnClickedButtonStrobe();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
