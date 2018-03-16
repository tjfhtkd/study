#pragma once

#include "stdafx.h"

#pragma comment(lib, "ws2_32")

class ClientList;

class MyNetConnector
{
public:
	MyNetConnector(INT clientQueueSize = SOMAXCONN);
	
	// closesocket
	// WSAclean
	virtual ~MyNetConnector();
	
public:
	// data translation

	INT Startup(void);
	INT PrepareSocket(int af = AF_INET, int type = SOCK_STREAM, int protocol = 0);
	INT Bind(ADDRESS_FAMILY af = AF_INET, ULONG address = INADDR_ANY, USHORT port = SERVER_PORT);
	INT Listen(void);

	SOCKET* Accept(void);
	INT Connect(PCSTR serverAddr = SERVER_IP, USHORT port = SERVER_PORT, ADDRESS_FAMILY af = AF_INET);

	INT GetLastError(void);
	INT GetListeningSize(void);
	SOCKET* GetPreparedSocket(void);

private:
	INT		m_listenQueueSize;
	INT		m_recentErrCode;
	SOCKET	m_listeningSock;
};

