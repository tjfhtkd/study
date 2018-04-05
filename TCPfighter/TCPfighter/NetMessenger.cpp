#include "stdafx.h"
#include "PacketDefine.h"
#include "CStreamSQ.h"
#include "PacketAnalyzer.h"
#include "NetMessenger.h"


NetMessenger::NetMessenger()
{
	m_sendBuffer		= new CStreamSQ(10000);
	m_recvBuffer			= new CStreamSQ(10000);
	m_packetAnalyzer	= new PacketAnalyzer();
	IsWouldBlock		= false;
}

NetMessenger::~NetMessenger()
{
	if (m_sendBuffer != nullptr)
	{
		delete m_sendBuffer;
		m_sendBuffer = nullptr;
	}

	if (m_recvBuffer != nullptr)
	{
		delete m_recvBuffer;
		m_recvBuffer = nullptr;
	}

	if (m_packetAnalyzer != nullptr)
	{
		delete m_packetAnalyzer;
		m_packetAnalyzer = nullptr;
	}
}

bool NetMessenger::SaveSendMsg(IN stPacket_ArgCollectionBox* netArgBox)
{
	char* buf = nullptr;
	int bufLen;
	bool bEncoded = m_packetAnalyzer->Encode(&buf, &bufLen, netArgBox->Direction, netArgBox->X, netArgBox->Y, netArgBox->PacketType);
	if (bEncoded == false || buf == nullptr)
	{
		return false;
	}

	if (m_sendBuffer->Enqueue(buf, bufLen) != bufLen)
	{
		// send Q 가득 참.
		return false;
	}
	return true;
}

INT NetMessenger::SendMsg(SOCKET sock)
{
	if (IsWouldBlock == true)
	{
		return -2;
	}

	char buf[5000];
	int len	= m_sendBuffer->Peek(buf, m_sendBuffer->GetUseSize());
	len = send(sock, buf, len, 0);
	if (len == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			IsWouldBlock = true;
			return WSAEWOULDBLOCK;
		}
		return SOCKET_ERROR;
	}
	m_sendBuffer->RemoveData(len);
	//len = m_sendBuffer->Dequeue(buf, len);
	return 0;
}

bool NetMessenger::RecvMsg(OUT int* out_type, OUT stPacket_ArgCollectionBox* out_netArgBox)
{
	if (out_type == nullptr || out_netArgBox == nullptr)
	{
		return false;
	}

	int packetSize = m_packetAnalyzer->Decode(m_recvBuffer->GetReadBufferPtr(), out_netArgBox);
	if (packetSize == -1)
	{
		return false;
	}

	m_recvBuffer->RemoveData(
		sizeof(st_NETWORK_PACKET_HEADER)
		+ packetSize
		+ sizeof(dfNETWORK_PACKET_END)
	);
	return true;
}

// 위임.
// 이유1 : ring buffer를 관리하는 것은 메신저 역할이고, 그 오류처리를 하는건 NetProcessor 역할이다.
//			따라서 NetProcessor 안에서 링버퍼를 직접 다루지는 않는다.
// 이유2 : ring buffer는 외부에서 맘대로 Dequeue 해선 안된다.
// 이유3 : ring buffer가 Messenger에 있어야 Send, Get, Encode, Decode를 수행하기 용이해진다.
// friend 써서 이 함수를 쓰는 유일한 클래스인 NetProcessor와 연관을 맺을 수 있긴 하겠지만, 결합도가...
bool NetMessenger::SaveRecvMsg(IN char* recvPacket, IN int recvLen)
{
	// recvPacket 값과 recvLen 값의 불일치 오류도 있음.
	// 근데 그건 못잡음. recvPacket에 들어있는 데이터 끝엔 '\0' 가 없을테니까.
	// 줄 때 넣어서 줘야한다는 규칙을 정해버리면 쓰는 쪽이 이 클래스 사용법에 너무 종속적이게 됨.
	// 그냥 교정하지 말고 외부에서 오류처리로 인식하게 하는 게 나음.
	if(recvPacket == nullptr)
	{
		return false;
	}

	if (recvLen <= 0)
	{
		return false;
	}

	if (m_recvBuffer->Enqueue(recvPacket, recvLen) != recvLen)
	{
		// recv Q 가득 참.
		return false;
	}
	return true;
}