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
// 수정해야할 내용 -> 메시지를 문자열이 아니라 ProtocolPacker 같은걸 통해서 간접적으로 받게 함
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

// 연습 -> 현재 Client마다 Thread가 생성됨.
//           각 클라이언트의 Thread는 recv를 block 상태로 기다림.
// 이후 -> Non block으로 하고 Client마다 Thread가 아닌 형태로 해야함.
void MyNetMessenger::Recv(void)
{
	m_bThreadRunning = true;
	INT retVal;
	while (m_bThreadRunning)
	{
		//retVal = recv(*m_connector->GetPreparedSocket(), 1);
	}
}