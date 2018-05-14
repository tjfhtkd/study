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
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					continue;
				}
				ResponseExitRoom(session);
				// '0' means gracefully closed.
				// 'SOCKET_ERROR' eq. -1, means something wrong.
				netMng->RemoveSession(session);
			}
			else
			{
				session->recvQ.Enqueue(buf, recvLen);
				if (IsIntactPacket(*session) == true)
				{
					AnalysePacket(*session);
				}
				else
				{
					// ����� ������ �ְų� ���� ��Ŷ�� �������ȴ�. �ۺ��� ������
					ResponseExitRoom(session);
					netMng->RemoveSession(session);
				}
			}
		}
	}

	for (kks::Session* session : wSessions)
	{
		kks::RingBuf localBuf(session->sendQ.GetUseSize());
		localBuf.MoveWritePos(session->sendQ.GetUseSize());
		session->sendQ << localBuf;
		// ���ʿ� ������ ��鸸 �ɷ������Ƿ� �׳� send �� ����
		while (localBuf.IsEmpty() == false)
		{
			// �۾� �� -> send�� �� ���� �� ������ �� ó��
			int sendSize = send(session->sock, localBuf.GetReadBufferPtr(), localBuf.GetUseSize(), 0);
		}
	}
}

void NetPacketProcessor::ResponseLogIn(kks::Session& session)
{
	BYTE result;
	WCHAR nickName[15];
	session.recvQ << &nickName;
	if (m_netMng->AddSession(session.sock, nickName) == true)
	{
		// session ��ü�� ptr�� ��� Ÿ�� �°Ŷ� �׳� �ٷ� uid �... ������ �ſ� �ر�������
		// �׳� �ر����� �迡 �� ��..
		result = df_RESULT_LOGIN_OK;
	}
	else
	{
		// ���� ���� ����� �ʰ����� ��Ȳ�� ����. �� ������ ���� ������ ��� �ø�...
		result = df_RESULT_LOGIN_DNICK;
	}

	// ���� �ͼ� üũ   05.12  a.m. 12:18
	st_PACKET_HEADER header;
	header.byCode = dfPACKET_CODE;
	header.wMsgType = df_RES_LOGIN;
	header.wPayloadSize = sizeof(BYTE) + sizeof(DWORD);
	header.byCheckSum = (MakeCheckSum(df_RES_ROOM_LEAVE) + MakeCheckSum(result) + MakeCheckSum(session.uid)) % 256;
	session.sendQ >> header >> &result >> &session.uid;
}

bool NetPacketProcessor::IsIntactPacket(kks::Session& session)
{
	if (session.recvQ.GetUseSize() <= sizeof(st_PACKET_HEADER))
	{
		return true;
	}

	int bufSize = session.recvQ.GetUseSize();
	kks::RingBuf localBuf(bufSize + 1);
	session.recvQ.Peek(localBuf.GetReadBufferPtr(), bufSize);
	
	// ������ ��Ŷ ������ �ݵ�� �ϼ��� ��Ŷ �����̹Ƿ� ����� ���� ���� �׻� �Ǿƾ� �Ѵ�.
	// ���� �װ� ������ �Ǵ°�..?
	st_PACKET_HEADER header;
	localBuf << &header;
	if (header.byCode != dfPACKET_CODE)
	{
		// strange session OR logic err
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
	kks::RingBuf localBuf(session.recvQ.GetUseSize() + 1);
	int readSize = session.recvQ.Peek(localBuf.GetReadBufferPtr(), localBuf.GetBufferSize());
	// Peek ��ü�� char* buf�� �޾Ƽ� �ٷ� memcpy �����°Ŵٺ��� localBuf�� ���� front, rear ��ȭ�� �Ȼ���.
	localBuf.MoveWritePos(readSize);
	if (localBuf.GetUseSize() <= sizeof(st_PACKET_HEADER))
	{
		return;
	}

	st_PACKET_HEADER header;
	localBuf << &header;
	if (localBuf.GetUseSize() < header.wPayloadSize)
	{
		return;
	}

	switch (header.wMsgType)
	{
	case df_REQ_LOGIN:
		//RequestLogIn();
		//break;
	case df_RES_LOGIN:
		ResponseLogIn(session);
		break;

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

	default:
		break;
	}
}

bool NetPacketProcessor::ResponseExitRoom(kks::Session* leavingSession)
{
	DWORD roomID = leavingSession->roomNo;

	// �κ� �ִ� ������ ������.
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
		localBuf >> &header >> &payload;
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
