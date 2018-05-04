#pragma once

class NetManager;

class NetPacketProcessor
{
public:
	NetPacketProcessor(NetManager* netMng);
	virtual ~NetPacketProcessor();

public:
	void ProcessSessionRequest(std::list<kks::Session*>& rSessions, std::list<kks::Session*>& wSessions);
	
	// �α���
	bool RequestLogIn();
	bool ResponseLogIn();

	// ��ȭ�� ���
	bool RequestRoomList();
	bool ResponseRoomList();

	// ��ȭ�� ����
	bool RequestRoomCreation();
	bool ResponseRoomCreation();

	// ��ȭ�� ����
	bool ResponseRoomDelete();

	// ��ȭ�� ����
	bool RequestJoinRoom();
	bool ResponseJoinRoom();

	// ��ȭ�� ����
	bool RequestExitRoom();
	bool ResponseExitRoom();

	// ä�� �޽��� �۽�/����
	bool RequestSendMsg();
	bool ResponseSendMsg();

	// Ÿ ����� ����
	bool ResponseUserEnter();

private:
	bool CheckHeader(CStreamSQ& buf);
	bool IsNormalityChecksum(st_PACKET_HEADER& header);
	SOCKET Accept(SOCKET listenSock);

private:
	NetManager* m_netMng;
};