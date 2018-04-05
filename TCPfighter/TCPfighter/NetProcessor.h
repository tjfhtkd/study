#pragma once

class NetConnector;
class INetReactor;
class NetMessenger;


class NetProcessor
{
public:
	NetProcessor();
	virtual ~NetProcessor();

public:
	bool RegistNetworkReactor(INetReactor* reactor);
	bool ConnectToServer(HWND hWnd, WCHAR* serverIP, INT serverPort);
	INT ProcessNetwork(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	SOCKET GetServerSock(void);

private:
	NetConnector*	m_netConn;
	INetReactor*		m_reactor;
	NetMessenger*	m_messenger;
};