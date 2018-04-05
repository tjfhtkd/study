#pragma once
class NetConnector
{
public:
	NetConnector();
	virtual ~NetConnector();

public:
	const static int WM_SOCK = WM_USER + 1;

	// 1. 소켓 라이브러리 열기.
	// 2. 소켓 준비
	// 3. 연결
	// 4. WSAAsyncSelect로 변경
	INT Connect(HWND hWnd, WCHAR* serverIP, INT serverPort);

public:
	SOCKET	ServerSock;

private:
	bool		m_bWsaOpend;
};