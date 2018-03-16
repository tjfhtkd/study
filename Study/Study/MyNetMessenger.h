#pragma once

#include <queue>

class MyNetConnector;
class thread;

// 내 프로젝트에서 주고받는 전체 메시지를 통괄 관리하는 클래스
class MyNetMessenger
{
public:
	MyNetMessenger(MyNetConnector* connector);
	virtual ~MyNetMessenger();

public:
	INT SendMsg(CONST SOCKET* sendTo, CONST CHAR* msg, CONST INT msgLen, CONST INT flags);
	WCHAR* GetMsg(void);

private:
	void Recv(void);

private:
	std::queue<stGeneralStamp*>	m_msgQ;
	MyNetConnector*				m_connector;
	
	std::thread*	m_recvThread;
	bool			m_bThreadRunning;
};