#include "stdafx.h"

#include "ClientList.h"
#include "MyNetConnector.h"


MyNetConnector::MyNetConnector(INT listenQueueSize)
{
	m_listeningSock = -1;
	m_recentErrCode = -1;
	if (listenQueueSize > 0)
	{
		m_listenQueueSize = listenQueueSize;
	}
	else
	{
		m_listenQueueSize = SOMAXCONN;
	}
}


MyNetConnector::~MyNetConnector()
{
	if (m_listeningSock != -1)
	{
		closesocket(m_listeningSock);
	}
	WSACleanup();
}

////////////////////////////////////// Public //////////////////////////////////////
INT MyNetConnector::Startup(void)
{
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		m_recentErrCode = WSAGetLastError();
		return m_recentErrCode;
	}
	return 0;
}

INT MyNetConnector::PrepareSocket(int af, int type, int protocol)
{
	m_listeningSock = socket(af, type, protocol);
	if (m_listeningSock == INVALID_SOCKET)
	{
		m_listeningSock = -1;
		m_recentErrCode = WSAGetLastError();
		return m_recentErrCode;
	}
	return 0;
}

INT MyNetConnector::Bind(ADDRESS_FAMILY af, ULONG address, USHORT port)
{
	SOCKADDR_IN addr;
	addr.sin_family	= af;
	addr.sin_port		= htons(port);
	addr.sin_addr.S_un.S_addr	= htonl(address);
	if (bind(m_listeningSock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		m_recentErrCode = WSAGetLastError();
		return m_recentErrCode;
	}
	return 0;
}

INT MyNetConnector::Listen(void)
{
	if (listen(m_listeningSock, m_listenQueueSize) == SOCKET_ERROR)
	{
		m_recentErrCode = WSAGetLastError();
		return m_recentErrCode;
	}
	return 0;
}

SOCKET* MyNetConnector::Accept(void)
{
	SOCKADDR_IN addr;
	int addrLen = sizeof(addr);

	SOCKET* clientSock = (SOCKET*)malloc(sizeof(SOCKET));
	*clientSock = accept(m_listeningSock, (SOCKADDR*)&addr, &addrLen);
	if (*clientSock == INVALID_SOCKET)
	{
		m_recentErrCode = WSAGetLastError();
		return nullptr;
	}
	return clientSock;
}

INT MyNetConnector::Connect(PCSTR serverAddr, USHORT port, ADDRESS_FAMILY af)
{
	SOCKADDR_IN addr;
	ZeroMemory(&addr, sizeof(addr));

	addr.sin_family	= af;
	addr.sin_port		= htons(port);
	inet_pton(af, serverAddr ,&(addr.sin_addr.S_un.S_addr));

	if (connect(m_listeningSock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		m_recentErrCode = WSAGetLastError();
		return m_recentErrCode;
	}

	return 0;
}

INT MyNetConnector::GetLastError(void)
{
	return m_recentErrCode;
}

INT MyNetConnector::GetListeningSize(void)
{
	return m_listenQueueSize;
}

SOCKET* MyNetConnector::GetPreparedSocket(void)
{
	return &m_listeningSock;
}