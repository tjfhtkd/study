#include "stdafx.h"
#include <thread>

#include "MyNetConnector.h"

#include "MyNetMessenger.h"


MyNetMessenger::MyNetMessenger(MyNetConnector* connector)
{
	if (connector != nullptr)
	{
		m_connector = connector;
	}
	//m_recvThread = new std::thread(&MyNetMessenger::Recv, this);
}

MyNetMessenger::~MyNetMessenger()
{
	/*if (m_recvThread->joinable() == true)
	{
		m_recvThread->join();
	}
	delete m_recvThread;
	m_recvThread = nullptr;*/
	m_connector = nullptr;
}

////////////////////////////////////// Public //////////////////////////////////////
// �����ؾ��� ���� -> �޽����� ���ڿ��� �ƴ϶� ProtocolPacker ������ ���ؼ� ���������� �ް� ��
INT MyNetMessenger::SendMsg(CONST SOCKET* sendTo, CONST CHAR* msg, CONST INT msgLen, CONST INT flags)
{
	INT sendingSize;
	sendingSize = send(*m_connector->GetPreparedSocket(), msg, msgLen, flags);
	if (sendingSize == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}
	return sendingSize;
}

WCHAR* MyNetMessenger::GetMsg(void)
{
	return nullptr;
}

////////////////////////////////////// Private //////////////////////////////////////

// ���� -> ���� Client���� Thread�� ������.
//           �� Ŭ���̾�Ʈ�� Thread�� recv�� block ���·� ��ٸ�.
// ���� -> Non block���� �ϰ� Client���� Thread�� �ƴ� ���·� �ؾ���.
void MyNetMessenger::Recv(void)
{
	m_bThreadRunning = true;
	INT retVal;
	while (m_bThreadRunning)
	{
		//retVal = recv(*m_connector->GetPreparedSocket(), 1);
	}
}