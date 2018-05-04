#include "stdafx.h"
#include "NetConnector.h"


NetConnector::NetConnector(INT port)
{
	m_port = port;
}

NetConnector::~NetConnector()
{
	closesocket(m_listenSock);
	WSACleanup();
	wprintf_s(L"Server Cleanup and close listen socket.\n");
}


INT NetConnector::Connect(void)
{
	wprintf_s(L"LIB Loading...");
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		wprintf_s(L" fail !!\n");
		return -1;
	}
	wprintf_s(L" success.\n\n");

	wprintf_s(L"Preparing listening socket...");
	m_listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listenSock == INVALID_SOCKET)
	{
		wprintf_s(L" fail !!\n");
		WSACleanup();
		return -1;
	}
	wprintf_s(L" success.\n\n");

	wprintf_s(L"Binding... ");
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_port);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(m_listenSock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		wprintf_s(L" fail !!\n");
		closesocket(m_listenSock);
		WSACleanup();
		return -1;
	}
	wprintf_s(L" success.\n\n");

	wprintf_s(L"Listening...");
	if (listen(m_listenSock, SOMAXCONN) == SOCKET_ERROR)
	{
		wprintf_s(L" fail !!\n");
		closesocket(m_listenSock);
		WSACleanup();
		return -1;
	}
	wprintf_s(L" success.\n\n");

	return 0;
}

SOCKET NetConnector::GetListenSocket(void)
{
	return m_listenSock;
}

INT NetConnector::GetPort(void)
{
	return m_port;
}

INT NetConnector::SetNonBlock(bool bNonBlock)
{
	ULONG on = (bNonBlock) ? 1 : 0;
	return ioctlsocket(m_listenSock, FIONBIO, &on);
}
