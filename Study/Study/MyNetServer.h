#pragma once

class MyNetConnector;

class MyNetServer
{
public:
	MyNetServer(MyNetConnector* connector);
	virtual ~MyNetServer();

public:
	// WSAstartup
	// prepare socket
	// binding
	// listening
	INT StartServer(ADDRESS_FAMILY af = AF_INET
		, ULONG bandAddress = INADDR_ANY
		, USHORT port = SERVER_PORT
		, int type = SOCK_STREAM
		, int protocol = 0);

	SOCKET* GetListenSock(void);

private:
	MyNetConnector*	m_connector;
	SOCKET*				m_listenSocket;
};

