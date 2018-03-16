#include "stdafx.h"

#include "ClientList.h"



ClientList::ClientList(INT queueSize)
{
	if (queueSize > 0)
	{
		m_clientQueueSize = queueSize;
	}
	else
	{
		m_clientQueueSize = SOMAXCONN;
	}
}

ClientList::~ClientList()
{
	SOCKET* sock;
	while (clients.size())
	{
		sock = clients.front();
		closesocket(*sock);
		clients.pop();
		free(sock);
		sock = nullptr;
	}
}

bool ClientList::Push(SOCKET* sock)
{
	if (clients.size() < m_clientQueueSize)
	{
		clients.push(sock);
		return true;
	}
	return false;
}

SOCKET* ClientList::Pop(void)
{
	if (clients.size() > 0)
	{
		return clients.front();
	}
	return nullptr;
}

INT ClientList::Size(void)
{
	return clients.size();
}

INT ClientList::Capacity(void)
{
	return m_clientQueueSize;
}
