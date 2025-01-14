
// VisionLightTestDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "VisionLightTest.h"
#include "VisionLightTestDlg.h"
#include "afxdialogex.h"
#include <sstream>     // std::istringstream을 사용하기 위해 포함
#include <algorithm>   // std::remove를 사용하기 위해 포함

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

void DataReceivedCallback(const char* data, DWORD size);

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CVisionLightTestDlg 대화 상자



CVisionLightTestDlg::CVisionLightTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VISIONLIGHTTEST_DIALOG, pParent)
	, m_default_set(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVisionLightTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_PORT, m_port);
	DDX_Control(pDX, IDC_COMBO_BIT_SECOND, m_bit_second);
	DDX_Control(pDX, IDC_COMBO_DATA_BIT, m_data_bit);

	DDX_Text(pDX, IDC_EDIT_WRITE_DATA, m_write_data);
	DDX_Text(pDX, IDC_EDIT_FRAME_REPETITION, m_frame_repetition);
	DDX_Text(pDX, IDC_EDIT_FRAME_MILLISECOND, m_frame_millisecond);

	DDX_Check(pDX, IDC_CHECK_DEFAULT_SET, m_default_set);
}

BEGIN_MESSAGE_MAP(CVisionLightTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CVisionLightTestDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CVisionLightTestDlg::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_WRITE, &CVisionLightTestDlg::OnBnClickedButtonWrite)
	ON_BN_CLICKED(IDC_CHECK_DEFAULT_SET, &CVisionLightTestDlg::OnBnClickedCheckDefaultSet)
	ON_BN_CLICKED(IDC_BUTTON_FRAME_REPETITION_SET, &CVisionLightTestDlg::OnBnClickedButtonFrameRepetitionSet)
	ON_BN_CLICKED(IDC_BUTTON_FRAME_MILLISECOND_SET, &CVisionLightTestDlg::OnBnClickedButtonFrameMillisecondSet)
	ON_BN_CLICKED(IDC_BUTTON_STROBE, &CVisionLightTestDlg::OnBnClickedButtonStrobe)
END_MESSAGE_MAP()


// CVisionLightTestDlg 메시지 처리기

BOOL CVisionLightTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	std::cout << "VisionLight_Version_1.1.1" << std::endl;
	std::cout << "24.10.24" << std::endl;
	InitComboBox();
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CVisionLightTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CVisionLightTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CVisionLightTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void DataReceivedCallback(const char* data, DWORD size) {
	CString receivedData(data, size);
	std::string str = CT2CA(receivedData);
	std::cout << str.c_str() << std::endl;
}

void CVisionLightTestDlg::InitComboBox()
{
	// 초기화할거임
	m_port.ResetContent();
	m_bit_second.ResetContent();
	m_data_bit.ResetContent();

	// 포트추가
	m_port.AddString(_T("COM1"));
	m_port.AddString(_T("COM2"));
	m_port.AddString(_T("COM3"));
	m_port.AddString(_T("COM4"));
	m_port.AddString(_T("COM5"));
	m_port.AddString(_T("COM6"));
	m_port.AddString(_T("COM7"));
	m_port.AddString(_T("COM8"));
	m_port.AddString(_T("COM9"));
	m_port.AddString(_T("COM10"));
	m_port.AddString(_T("COM11"));
	m_port.AddString(_T("COM12"));
	m_port.AddString(_T("COM13"));
	m_port.AddString(_T("COM14"));
	m_port.AddString(_T("COM15"));
	m_port.AddString(_T("COM16"));
	m_port.AddString(_T("COM17"));
	m_port.AddString(_T("COM18"));
	m_port.AddString(_T("COM19"));
	m_port.AddString(_T("COM20"));

	// 비트/초 추가
	m_bit_second.AddString(_T("75"));
	m_bit_second.AddString(_T("110"));
	m_bit_second.AddString(_T("134"));
	m_bit_second.AddString(_T("150"));
	m_bit_second.AddString(_T("300"));
	m_bit_second.AddString(_T("1200"));
	m_bit_second.AddString(_T("1800"));
	m_bit_second.AddString(_T("2400"));
	m_bit_second.AddString(_T("4800"));
	m_bit_second.AddString(_T("7200"));
	m_bit_second.AddString(_T("9600"));
	m_bit_second.AddString(_T("14400"));
	m_bit_second.AddString(_T("19200"));
	m_bit_second.AddString(_T("38400"));
	m_bit_second.AddString(_T("57600"));
	m_bit_second.AddString(_T("115200"));
	m_bit_second.AddString(_T("128000"));

	// 데이터비트
	m_data_bit.AddString(_T("4"));
	m_data_bit.AddString(_T("5"));
	m_data_bit.AddString(_T("6"));
	m_data_bit.AddString(_T("7"));
	m_data_bit.AddString(_T("8"));

	// 초기값 설정
	m_port.SetCurSel(0); 
	m_bit_second.SetCurSel(10);
	m_data_bit.SetCurSel(4);
}

void CVisionLightTestDlg::OnBnClickedButtonConnect()
{
	//CString str_port;
	CString str_bit_second;
	CString str_data_bit;

	int port_Idx = m_port.GetCurSel();
	int bit_second_Idx = m_bit_second.GetCurSel();
	int data_bit_Idx = m_data_bit.GetCurSel();

	// 인덱스가 유효한지 확인
	if (port_Idx != CB_ERR && bit_second_Idx != CB_ERR && data_bit_Idx != CB_ERR)
	{
		m_port.GetLBText(port_Idx, m_str_port);

		m_bit_second.GetLBText(bit_second_Idx, str_bit_second);
		m_nbit_second = _ttoi(str_bit_second);

		m_data_bit.GetLBText(data_bit_Idx, str_data_bit);
		m_ndata_bit = _ttoi(str_data_bit);

	}
	else
	{
		std::cout << "select error" << m_ndata_bit << std::endl;
		return;
	}

	std::string str = CT2CA(m_str_port);
	std::cout << "port: " << str.c_str() << " 비트/초: " << m_nbit_second << " 데이터 비트: " << m_ndata_bit << std::endl;

	// 포트 오픈
	if (m_serialComm.OpenPort(m_str_port, m_nbit_second, m_ndata_bit)) {
		std::cout << "Port opened successfully!" << std::endl;

		// 콜백 함수 설정
		m_serialComm.SetReadCallback(DataReceivedCallback);

		// 비동기 읽기 ㄱㄱ
		m_serialComm.StartAsyncRead();

		GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(TRUE);
	}
	else {
		AfxMessageBox(_T("Failed to open port!"));
	}
}

void CVisionLightTestDlg::OnBnClickedButtonDisconnect()
{
	// 비동기 해제
	m_serialComm.StopAsyncRead();
	// 포트 닫고
	m_serialComm.ClosePort();
	std::cout << "disconnect Port" << std::endl;

	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);
}

void CVisionLightTestDlg::OnBnClickedButtonWrite()
{
	UpdateData(TRUE);
	// CString 생성 (공백 및 쉼표로 구분된 16진수 문자열)
	//CString strHex = _T("0x02, N, 255, 0x41, 0x42, 0x03"); // 예제 문자열

	// CString을 std::string으로 변환
	std::string strHexStd(CT2A(m_write_data.GetString())); // m_write_data -> strHex로 변경
	std::istringstream iss(strHexStd);
	std::string token;

	std::string result; // 변환된 결과 문자열
	std::string originalWithResult; // 원본 값 + 변환된 결과

	while (std::getline(iss, token, ',')) {
		// 공백 제거
		token.erase(std::remove(token.begin(), token.end(), ' '), token.end());

		std::string convertedValue; // 변환된 값을 저장할 변수

		// "0x"로 시작하면 16진수로 처리
		if (token.find("0x") == 0 || token.find("0X") == 0) {
			int decimalValue = std::stoi(token, nullptr, 16);
			char character = static_cast<char>(decimalValue);
			convertedValue = std::string(1, character); // 변환된 문자 저장
			result += character; // 변환된 값을 최종 결과에 추가
		}
		// 숫자로만 구성된 경우 10진수로 처리
		else if (std::all_of(token.begin(), token.end(), ::isdigit)) {
			convertedValue = token; // 숫자를 그대로 문자열에 추가
			result += token;
		}
		// 그 외는 문자 그대로 추가
		else {
			convertedValue = token; // 그대로 추가
			result += token;
		}

		// 원본 값 + 변환된 값을 저장
		originalWithResult += token + " ";
	}

	// 최종 결과 출력
	//std::cout << "Converted Result: " << result << std::endl;
	std::cout << "Original with Converted Values: " << originalWithResult << "\n" << std::endl;

	// 시리얼 포트에 데이터 전송
	if (m_serialComm.WriteData(result.c_str(), result.size())) {
		// 전송된 데이터를 콘솔에 출력
		std::cout << "send: " << result.c_str() << std::endl;
	}
	else {
		AfxMessageBox(_T("Failed to send data!"));
	}

	/////////////////////////////////////////////////////
	//// 문자열을 헥사 코드로 변환해서 전송
	//std::vector<unsigned char> data(str.begin(), str.end());

	//// 시리얼 포트에 데이터 전송
	//if (m_serialComm.WriteData((const char*)data.data(), data.size())) {
	//	std::cout << "Sent: ";
	//	for (const auto& byte : data) {
	//		std::cout << std::hex << (int)byte << " ";
	//	}
	//	std::cout << std::endl;
	//}
	//else {
	//	AfxMessageBox(_T("Failed to send data!"));
	//}
	/////////////////////////////////////////////////////
	//
	//std::string str = CT2CA(m_write_data);

	//// 시리얼 포트에 데이터 전송
	//if (m_serialComm.WriteData(str.c_str(), str.size())) {
	//	// 전송된 데이터를 콘솔에 출력
	//	std::cout << "send: " << str.c_str() << std::endl;
	//}
	//else {
	//	AfxMessageBox(_T("Failed to send data!"));
	//}
	/////////////////////////////////////////////////////


	//const char* sendData = CT2A(m_write_data);

	//if (m_serialComm.WriteData(sendData, strlen(sendData))) {
	//	std::cout << sendData << std::endl;
	//}
	//else {
	//	AfxMessageBox(_T("Failed to send data!"));
	//}
	/////////////////////////////////////////////////////
}


void CVisionLightTestDlg::OnBnClickedCheckDefaultSet()
{
	UpdateData(TRUE);

	if (m_default_set) {
		GetDlgItem(IDC_CHECK_CH1_1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH1_2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH1_3)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH1_4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH1_5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH1_6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH1_7)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH1_8)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH1_9)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH1_10)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH1_11)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH1_12)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHECK_CH2_1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH2_2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH2_3)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH2_4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH2_5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH2_6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH2_7)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH2_8)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH2_9)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH2_10)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH2_11)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH2_12)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHECK_CH3_1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH3_2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH3_3)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH3_4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH3_5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH3_6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH3_7)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH3_8)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH3_9)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH3_10)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH3_11)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH3_12)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHECK_CH4_1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH4_2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH4_3)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH4_4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH4_5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH4_6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH4_7)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH4_8)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH4_9)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH4_10)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH4_11)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH4_12)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHECK_CH5_1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH5_2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH5_3)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH5_4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH5_5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH5_6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH5_7)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH5_8)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH5_9)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH5_10)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH5_11)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH5_12)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHECK_CH6_1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH6_2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH6_3)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH6_4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH6_5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH6_6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH6_7)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH6_8)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH6_9)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH6_10)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH6_11)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH6_12)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHECK_CH7_1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH7_2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH7_3)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH7_4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH7_5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH7_6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH7_7)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH7_8)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH7_9)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH7_10)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH7_11)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH7_12)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHECK_CH8_1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH8_2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH8_3)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH8_4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH8_5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH8_6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH8_7)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH8_8)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH8_9)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH8_10)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH8_11)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH8_12)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHECK_CH9_1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH9_2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH9_3)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH9_4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH9_5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH9_6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH9_7)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH9_8)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH9_9)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH9_10)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH9_11)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH9_12)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHECK_CH10_1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH10_2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH10_3)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH10_4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH10_5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH10_6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH10_7)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH10_8)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH10_9)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH10_10)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH10_11)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_CH10_12)->EnableWindow(FALSE);
	}
	else {
		GetDlgItem(IDC_CHECK_CH1_1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH1_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH1_3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH1_4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH1_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH1_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH1_7)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH1_8)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH1_9)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH1_10)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH1_11)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH1_12)->EnableWindow(TRUE);

		GetDlgItem(IDC_CHECK_CH2_1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH2_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH2_3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH2_4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH2_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH2_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH2_7)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH2_8)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH2_9)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH2_10)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH2_11)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH2_12)->EnableWindow(TRUE);

		GetDlgItem(IDC_CHECK_CH3_1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH3_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH3_3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH3_4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH3_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH3_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH3_7)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH3_8)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH3_9)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH3_10)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH3_11)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH3_12)->EnableWindow(TRUE);

		GetDlgItem(IDC_CHECK_CH4_1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH4_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH4_3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH4_4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH4_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH4_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH4_7)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH4_8)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH4_9)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH4_10)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH4_11)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH4_12)->EnableWindow(TRUE);

		GetDlgItem(IDC_CHECK_CH5_1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH5_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH5_3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH5_4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH5_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH5_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH5_7)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH5_8)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH5_9)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH5_10)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH5_11)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH5_12)->EnableWindow(TRUE);

		GetDlgItem(IDC_CHECK_CH6_1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH6_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH6_3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH6_4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH6_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH6_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH6_7)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH6_8)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH6_9)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH6_10)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH6_11)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH6_12)->EnableWindow(TRUE);

		GetDlgItem(IDC_CHECK_CH7_1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH7_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH7_3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH7_4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH7_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH7_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH7_7)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH7_8)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH7_9)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH7_10)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH7_11)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH7_12)->EnableWindow(TRUE);

		GetDlgItem(IDC_CHECK_CH8_1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH8_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH8_3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH8_4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH8_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH8_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH8_7)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH8_8)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH8_9)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH8_10)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH8_11)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH8_12)->EnableWindow(TRUE);

		GetDlgItem(IDC_CHECK_CH9_1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH9_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH9_3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH9_4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH9_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH9_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH9_7)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH9_8)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH9_9)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH9_10)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH9_11)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH9_12)->EnableWindow(TRUE);

		GetDlgItem(IDC_CHECK_CH10_1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH10_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH10_3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH10_4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH10_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH10_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH10_7)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH10_8)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH10_9)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH10_10)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH10_11)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_CH10_12)->EnableWindow(TRUE);
	}
}


void CVisionLightTestDlg::OnBnClickedButtonFrameRepetitionSet()
{
	UpdateData(TRUE);
	std::string str = CT2CA(m_frame_repetition);
	std::cout << "send frame repetition set: " << str.c_str() << std::endl;
}


void CVisionLightTestDlg::OnBnClickedButtonFrameMillisecondSet()
{
	UpdateData(TRUE);
	std::string str = CT2CA(m_frame_millisecond);
	std::cout << "send frame millisecond set: " << str.c_str() << std::endl;
}


void CVisionLightTestDlg::OnBnClickedButtonStrobe()
{

}


BOOL CVisionLightTestDlg::PreTranslateMessage(MSG* pMsg)
{
	// Enter 키를 감지하고 CEdit 컨트롤에 포커스가 있을 때만 처리
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		CWnd* pFocusWnd = GetFocus();
		if (pFocusWnd && pFocusWnd->GetDlgCtrlID() == IDC_EDIT_WRITE_DATA)
		{
			// 엔터 키를 누르면 버튼 클릭 메시지 전송 (버튼 ID를 변경)
			GetDlgItem(IDC_BUTTON_WRITE)->SendMessage(BM_CLICK);
			return TRUE; // 메시지 처리 완료
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


