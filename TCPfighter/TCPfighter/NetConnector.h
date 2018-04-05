#pragma once
class NetConnector
{
public:
	NetConnector();
	virtual ~NetConnector();

public:
	const static int WM_SOCK = WM_USER + 1;

	// 1. ���� ���̺귯�� ����.
	// 2. ���� �غ�
	// 3. ����
	// 4. WSAAsyncSelect�� ����
	INT Connect(HWND hWnd, WCHAR* serverIP, INT serverPort);

public:
	SOCKET	ServerSock;

private:
	bool		m_bWsaOpend;
};