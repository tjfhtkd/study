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
			char buf[4096];
			INT recvLen = recv(session->sock, buf, 4096, 0);
			if (recvLen == 0 || recvLen == SOCKET_ERROR)
			{
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					continue;
				}
				ResponseExitRoom(session);
				// '0' means gracefully closed.
				// 'SOCKET_ERROR' eq. -1, means something wrong.
				netMng->RemoveSession(session);
				wSessions.remove(session);
			}
			else
			{
				session->recvQ.Enqueue(buf, recvLen);
				if (IsIntactPacket(*session) == true)
				{
					AnalysePacket(*session);
				}
				//wprintf_s(L"RECV\n");
			}
		}
	}

	int sendCnt = 0;
	for (kks::Session* session : wSessions)
	{
		int localBufSize = session->sendQ.GetUseSize();
		kks::LocalBuf localBuf(localBufSize + 1);
		session->sendQ >> localBuf;
		// 애초에 멀쩡한 놈들만 걸러졌으므로 그냥 send 다 때림
		int sendSize = send(session->sock, (const char*)localBuf.GetReadBufferPtr(), (int)localBuf.GetUseSize(), 0);
		//int sendSize = send(session->sock, session->sendQ.GetReadBufferPtr(), session->sendQ.GetReadableSizeAtOneTime(), 0);
		if (sendSize == SOCKET_ERROR /*|| sendSize != localBuf.GetUseSize()*/)
		{
			// 뭔가 문제가 있다. 저쪽이나 이쪽이나 송신 버퍼가 가득 찬 경우이므로 굿바이.
			ResponseExitRoom(session);
			netMng->RemoveSession(session);
			//wprintf_s(L"SEND BUFFER IS FULL OR SOCKET_ERROR\n");
		}
		//session->sendQ.RemoveData(sendSize);
		/*else
		{
			wprintf_s(L"<%5d> SEND %s\n", ++sendCnt, localBuf.GetReadBufferPtr() + sizeof(st_PACKET_HEADER));
		}*/
	}
}

void NetPacketProcessor::ResponseLogIn(kks::Session& session)
{
	BYTE result;
	WCHAR nickName[15];
	session.recvQ << &nickName;
	if (m_netMng->AddSession(session.sock, nickName) == true)
	{
		// session 자체가 ptr로 계속 타고 온거라서 그냥 바로 uid 찌름... 구조가 매우 해괴해졌음
		// 그냥 해괴해진 김에 막 함..
		result = df_RESULT_LOGIN_OK;
	}
	else
	{
		// 현재 나는 사용자 초과같은 상황이 없음. 걍 서버가 죽을 때까지 계속 늘림...
		result = df_RESULT_LOGIN_DNICK;
	}

	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.wMsgType = df_RES_LOGIN;
	header.wPayloadSize = sizeof(result) + sizeof(session.uid);
	header.byCheckSum = (BYTE)((MakeCheckSum(df_RES_LOGIN) + MakeCheckSum(result) + MakeCheckSum(session.uid)) % 256);
	session.sendQ >> header >> result >> session.uid;
}

void NetPacketProcessor::EchoTest(kks::Session& session)
{
	st_PACKET_HEADER header;
	session.recvQ << &header;

	WCHAR echoMsg[128];
	session.recvQ << &echoMsg;

	session.sendQ >> header >> echoMsg;
}

bool NetPacketProcessor::IsIntactPacket(kks::Session& session)
{
	if (session.recvQ.GetUseSize() <= sizeof(st_PACKET_HEADER))
	{
		return false;
	}
	
	// 빼가는 패킷 단위는 반드시 완성된 패킷 단위이므로 헤더를 읽을 때는 항상 옳아야 한다.
	// 정말 그게 보장이 되는가..?
	st_PACKET_HEADER header;
	session.recvQ.Peek((char*)&header, sizeof(st_PACKET_HEADER));
	if (header.byCode != dfPACKET_CODE)
	{
		// strange session OR logic err
		return false;
	}

	if (session.recvQ.GetUseSize() - sizeof(st_PACKET_HEADER) < header.wPayloadSize)
	{
		return false;
	}

	return true;
}

SOCKET NetPacketProcessor::Accept(SOCKET listenSock)
{
	SOCKADDR_IN addr;
	INT addrLen = sizeof(addr);
	SOCKET client = accept(listenSock, (SOCKADDR*)&addr, &addrLen);
	return client;
}

void NetPacketProcessor::AnalysePacket(kks::Session& session)
{
	st_PACKET_HEADER header;
	session.recvQ << &header;

	switch (header.wMsgType)
	{
	case df_RES_LOGIN:
		ResponseLogIn(session);
		break;
		/*
	case df_REQ_ROOM_LIST:
		break;

	case df_RES_ROOM_LIST:
		break;

	case df_REQ_ROOM_CREATE:
		break;

	case df_RES_ROOM_CREATE:
		break;

	case df_REQ_ROOM_ENTER:
		break;

	case df_RES_ROOM_ENTER:
		break;

	case df_REQ_CHAT:
		break;

	case df_RES_CHAT:
		break;

	case df_REQ_ROOM_LEAVE:
		break;

	case df_RES_ROOM_LEAVE:
		break;

	case df_RES_ROOM_DELETE:
		break;

	case df_RES_USER_ENTER:
		break;
		*/
	case 15:	// ECHO test
		//EchoTest(session);
		WCHAR echoMsg[128];
		session.recvQ << &echoMsg;

		session.sendQ >> header >> echoMsg;
		break;

	default:
		break;
	}
}

DWORD NetPacketProcessor::MakeCheckSum(DWORD in_checkSum, kks::LocalBuf& buf, int size)
{
	unsigned char* cursor = (unsigned char*)buf.GetReadBufferPtr();
	for (int i = 0; i < size; i++) {

		in_checkSum += *cursor;
		cursor++;
	}
	return in_checkSum % 256;
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
