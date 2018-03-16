#pragma once
#include "stdafx.h"

class ClientList
{
public:
	ClientList(INT queueSize = SOMAXCONN);
	virtual ~ClientList();

public:
	bool Push(SOCKET* sock);
	SOCKET* Pop(void);

	INT Size(void);
	INT Capacity(void);

private:
	SIZE_T		m_clientQueueSize;
	
	std::queue<SOCKET*> clients;
};

