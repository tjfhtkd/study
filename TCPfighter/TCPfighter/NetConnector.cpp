#include "stdafx.h"
#include "NetConnector.h"

#pragma warning(disable:4996)

NetConnector::NetConnector()
{
	m_bWsaOpend = false;
	ServerSock = -1;
}


NetConnector::~NetConnector()
{
	if (m_bWsaOpend == true)
	{
		WSACleanup();
	}
}

INT NetConnector::Connect(HWND hWnd, WCHAR* serverIP, INT serverPort)
{
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0 && m_bWsaOpend == false)
	{
		return -2;
	}
	m_bWsaOpend = true;

	SOCKET serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSock == INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}

	SOCKADDR_IN server;
	server.sin_family	= AF_INET;
	server.sin_port	= htons(serverPort);
	InetPton(AF_INET, serverIP, &server.sin_addr.S_un);
	if (connect(serverSock, (SOCKADDR*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		closesocket(serverSock);
		return SOCKET_ERROR;
	}

	int retVal = WSAAsyncSelect(serverSock, hWnd, WM_SOCK, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE);
	if (retVal == SOCKET_ERROR)
	{
		closesocket(serverSock);
		return SOCKET_ERROR;
	}

	ServerSock = serverSock;
	return 0;
}