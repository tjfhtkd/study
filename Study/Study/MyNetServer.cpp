#include "stdafx.h"
#include "MyNetConnector.h"

#include "MyNetServer.h"


MyNetServer::MyNetServer(MyNetConnector* connector)
{
	if (connector != nullptr)
	{
		m_connector = connector;
	}
}

MyNetServer::~MyNetServer()
{
}

////////////////////////////////////// Public //////////////////////////////////////
INT MyNetServer::StartServer(ADDRESS_FAMILY af, ULONG bandAddress, USHORT port, int type, int protocol)
{
	if (m_connector == nullptr)
	{
		return -1224;
	}

	if (m_connector->Startup() != 0)
	{
		wprintf_s(L"\n[Startup Fail]\n");
		return m_connector->GetLastError();
	}

	if (m_connector->PrepareSocket(af, type, protocol) != 0)
	{
		wprintf_s(L"\n[Socket Preparing Fail]\n");
		return m_connector->GetLastError();
	}

	if (m_connector->Bind(af, bandAddress, port) != 0)
	{
		wprintf_s(L"\n[Bind Fail]\n");
		return m_connector->GetLastError();
	}

	if (m_connector->Listen() != 0)
	{
		wprintf_s(L"\n[Listen Fail]\n");
		return m_connector->GetLastError();
	}

	m_listenSocket = m_connector->GetPreparedSocket();
	return 0;
}

SOCKET* MyNetServer::GetListenSock(void)
{
	return m_listenSocket;
}
