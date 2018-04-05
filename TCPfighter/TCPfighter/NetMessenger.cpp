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
		// send Q ���� ��.
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

// ����.
// ����1 : ring buffer�� �����ϴ� ���� �޽��� �����̰�, �� ����ó���� �ϴ°� NetProcessor �����̴�.
//			���� NetProcessor �ȿ��� �����۸� ���� �ٷ����� �ʴ´�.
// ����2 : ring buffer�� �ܺο��� ����� Dequeue �ؼ� �ȵȴ�.
// ����3 : ring buffer�� Messenger�� �־�� Send, Get, Encode, Decode�� �����ϱ� ����������.
// friend �Ἥ �� �Լ��� ���� ������ Ŭ������ NetProcessor�� ������ ���� �� �ֱ� �ϰ�����, ���յ���...
bool NetMessenger::SaveRecvMsg(IN char* recvPacket, IN int recvLen)
{
	// recvPacket ���� recvLen ���� ����ġ ������ ����.
	// �ٵ� �װ� ������. recvPacket�� ����ִ� ������ ���� '\0' �� �����״ϱ�.
	// �� �� �־ ����Ѵٴ� ��Ģ�� ���ع����� ���� ���� �� Ŭ���� ������ �ʹ� �������̰� ��.
	// �׳� �������� ���� �ܺο��� ����ó���� �ν��ϰ� �ϴ� �� ����.
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
		// recv Q ���� ��.
		return false;
	}
	return true;
}