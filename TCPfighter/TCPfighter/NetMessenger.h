#pragma once

class CStreamSQ;
class PacketAnalyzer;

extern struct stPacket_ArgCollectionBox;

class NetMessenger
{
public:
	NetMessenger();
	virtual ~NetMessenger();

public:
	bool SaveSendMsg(IN stPacket_ArgCollectionBox* netArgBox);
	INT SendMsg(SOCKET sock);

	bool SaveRecvMsg(IN char* recvPacket, IN int recvLen);
	bool RecvMsg(OUT int* out_type, OUT stPacket_ArgCollectionBox* out_netArgBox);

public:
	bool	IsWouldBlock;

private:
	CStreamSQ*		m_sendBuffer;
	CStreamSQ*		m_recvBuffer;
	PacketAnalyzer*	m_packetAnalyzer;
};