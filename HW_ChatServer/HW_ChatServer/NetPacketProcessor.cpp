#include "stdafx.h"
#include "NetManager.h"
#include "NetPacketProcessor.h"

NetPacketProcessor::NetPacketProcessor(NetManager* netMng)
	:m_netMng(netMng)
{
}

NetPacketProcessor::~NetPacketProcessor()
{
}

void NetPacketProcessor::ProcessSessionRequest(std::list<kks::Session*>& rSessions, std::list<kks::Session*>& wSessions)
{
	NetManager* netMng = m_netMng;
	CStreamSQ localBuf(1024);
	for (kks::Session* session : rSessions)
	{
		INT recvLen = recv(session->sock, localBuf.GetBufferPtr(), localBuf.GetReadableSizeAtOneTime(), 0);
		if (lstrcmpW(session->nickName, L"LISTEN_SOCK") == 0)
		{
			SOCKET client = Accept(session->sock);
			if (client == SOCKET_ERROR)
			{
				continue;
			}
			netMng->AddSession();
		}
		else
		{
			if (CheckHeader(localBuf) == false)
			{
				continue;
			}
		}

	}
}

bool NetPacketProcessor::CheckHeader(CStreamSQ& buf)
{
	st_PACKET_HEADER headerFrame;
	if (IsNormalityChecksum(headerFrame) == false)
	{
		return false;
	}

	return true;
}

bool NetPacketProcessor::IsNormalityChecksum(st_PACKET_HEADER& header)
{
	return false;
}

SOCKET NetPacketProcessor::Accept(SOCKET listenSock)
{
	SOCKADDR_IN addr;
	INT addrLen = sizeof(addr);
	SOCKET client = accept(listenSock, (SOCKADDR*)&addr, &addrLen);
	return client;
}