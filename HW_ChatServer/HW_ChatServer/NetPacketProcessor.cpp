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
	for (kks::Session* session : rSessions)
	{
		if (lstrcmpW(session->nickName, L"LISTEN_SOCK") == 0)
		{
			SOCKET client = Accept(session->sock);
			if (client == SOCKET_ERROR)
			{
				continue;
			}
			netMng->AddSession(client, nullptr);
		}
		else
		{
			char buf[2048];
			INT recvLen = recv(session->sock, buf, 2048, 0);
			if (recvLen == 0 || recvLen == SOCKET_ERROR)
			{
				ResponseExitRoom(session);
				// '0' means gracefully closed.
				// 'SOCKET_ERROR' eq. -1, means something wrong.
				netMng->RemoveSession(session);
			}
			else
			{
				session->recvQ.Enqueue(buf, recvLen);
			}
		}
	}

	for (kks::Session* session : wSessions)
	{

	}
}

SOCKET NetPacketProcessor::Accept(SOCKET listenSock)
{
	SOCKADDR_IN addr;
	INT addrLen = sizeof(addr);
	SOCKET client = accept(listenSock, (SOCKADDR*)&addr, &addrLen);
	return client;
}

bool NetPacketProcessor::ResponseExitRoom(kks::Session* leavingSession)
{
	DWORD roomID = leavingSession->roomNo;

	// 로비에 있던 유저가 나갔다.
	if (roomID == 0)
	{
		return false;
	}

	DWORD payload = leavingSession->uid;
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.wMsgType = df_RES_ROOM_LEAVE;
	header.wPayloadSize = sizeof(DWORD);
	header.byCheckSum = (MakeCheckSum(df_RES_ROOM_LEAVE) + MakeCheckSum(payload)) % 256;

	int sendRetSize;
	int sendMsgSize = sizeof(DWORD) + sizeof(st_PACKET_HEADER);
	NetManager* netMng = m_netMng;
	kks::Room* enjoyedRoom = m_netMng->GetRoomInfo(roomID);
	kks::Session* session;
	CStreamSQ localBuf(2048);
	for (int i = 0; i < enjoyedRoom->chatterJoinCnt; i++)
	{
		localBuf >> header >> payload;
		session = netMng->GetSession(enjoyedRoom->uidList[i]);
		if (session->uid == leavingSession->uid)
		{
			continue;
		}
		session->sendQ << localBuf;
		localBuf.ClearBuffer();
	}
	return true;
}
