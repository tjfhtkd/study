#include "stdafx.h"
#include "MyNetConnector.h"

#include "MyNetClient.h"


MyNetClient::MyNetClient(MyNetConnector* connector)
{
	if (connector != nullptr)
	{
		m_connector = connector;
	}
	m_serverSocket = nullptr;
}


MyNetClient::~MyNetClient()
{
}

////////////////////////////////////// Public //////////////////////////////////////
INT MyNetClient::ConnectServer(PCSTR serverAddr, USHORT port, ADDRESS_FAMILY af, int type, int protocol)
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

	if (m_connector->Connect(serverAddr, port, af) != 0)
	{
		wprintf_s(L"\n[Socket Preparing Fail]\n");
		return m_connector->GetLastError();
	}

	m_serverSocket = m_connector->GetPreparedSocket();
	return 0;
}

SOCKET* MyNetClient::GetServer(void)
{
	return m_serverSocket;
}
