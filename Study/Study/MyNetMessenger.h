#pragma once

#include <queue>

class MyNetConnector;
class thread;

// �� ������Ʈ���� �ְ�޴� ��ü �޽����� ��� �����ϴ� Ŭ����
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