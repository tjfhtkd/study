#include "stdafx.h"
#include "NetConnector.h"
#include "INetReactor.h"
#include "NetMessenger.h"
#include "NetProcessor.h"


NetProcessor::NetProcessor()
{
	m_netConn		= new NetConnector();
	m_messenger	= new NetMessenger();
}

NetProcessor::~NetProcessor()
{
	if (m_netConn != nullptr)
	{
		delete m_netConn;
		m_netConn = nullptr;
	}

	if (m_messenger != nullptr)
	{
		delete m_messenger;
		m_messenger = nullptr;
	}
}

bool NetProcessor::ConnectToServer(HWND hWnd, WCHAR* serverIP, INT serverPort)
{
	if (m_netConn->Connect(hWnd, serverIP, serverPort) != 0)
	{
		return false;
	}
	return true;
}

bool NetProcessor::RegistNetworkReactor(INetReactor* reactor)
{
	if (reactor != nullptr)
	{
		m_reactor = reactor;
		return true;
	}
	return false;
}

INT NetProcessor::ProcessNetwork(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message != NetConnector::WM_SOCK)
	{
		return -1;
	}

	if (WSAGETSELECTERROR(lParam))
	{
		m_reactor->ReactWhenClose(m_netConn->ServerSock);
		return WSAGetLastError();
	}

	int retVal = 0;
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CONNECT:
	{
		m_reactor->ReactWhenConnected(m_messenger);
		break;
	}

	case FD_READ:
	{
		char buf[1000];
		int recvVal = recv(m_netConn->ServerSock, buf, 1000, 0);

		// 읽기 이상
		if (recvVal == SOCKET_ERROR)
		{
			// 읽기 보류
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				break;
			}
			// 소켓 오류로 인한 읽기 이상
			recvVal = SOCKET_ERROR;
			break;
		}

		if (m_messenger->SaveRecvMsg(buf, recvVal) == false)
		{
			// 큐가 가득 참. 게임에서는 이상한 상황.
			// 연결을 끊도록 하위 클래스에 권장한다.
			m_reactor->ReactWhenClose(m_netConn->ServerSock);
			retVal = -1;
			break;
		}
		m_reactor->ReactWhenReadable(m_messenger);
		retVal = FD_READ;
		break;
	}

	case FD_WRITE:
	{
		if (m_messenger->IsWouldBlock == true)
		{
			m_messenger->IsWouldBlock = false;
			m_reactor->ReactWhenWritable(m_messenger);
		}
		retVal = FD_WRITE;
		break;
	}

	case FD_CLOSE:
	{
		m_reactor->ReactWhenClose(m_netConn->ServerSock);
		retVal = FD_CLOSE;
		break;
	}

	default:
		retVal = -1;
		break;
	}
	return retVal;
}

SOCKET NetProcessor::GetServerSock(void)
{
	return m_netConn->ServerSock;
}

