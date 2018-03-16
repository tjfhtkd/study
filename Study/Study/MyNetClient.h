#pragma once

class MyNetConnector;

class MyNetClient
{
public:
	MyNetClient(MyNetConnector* connector);
	virtual ~MyNetClient();

public:
	// WSAstartup
	// prepare socket
	// connection
	INT ConnectServer(PCSTR serverAddr = SERVER_IP
		, USHORT port = SERVER_PORT
		, ADDRESS_FAMILY af = AF_INET
		, int type = SOCK_STREAM
		, int protocol = 0);

	SOCKET* GetServer(void);

private:
	MyNetConnector*	m_connector;
	SOCKET*				m_serverSocket;
};

