#pragma once

class NetConnector
{
public:
	NetConnector(INT port);
	virtual ~NetConnector();

public:
	INT		Connect(void);
	SOCKET	GetListenSocket(void);
	INT		GetPort(void);
	INT		SetNonBlock(bool bNonBlock);

private:
	SOCKET	m_listenSock;
	INT		m_port;
};