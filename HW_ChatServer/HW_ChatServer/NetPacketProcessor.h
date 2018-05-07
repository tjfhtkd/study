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
	bool ResponseExitRoom(kks::Session* leavingSession);

	// ä�� �޽��� �۽�/����
	bool RequestSendMsg();
	bool ResponseSendMsg();

	// Ÿ ����� ����
	bool ResponseUserEnter();

private:
	/*bool CheckHeader(CStreamSQ& buf);
	bool IsNormalityChecksum(st_PACKET_HEADER& header);*/
	SOCKET Accept(SOCKET listenSock);

	template<class T>
	DWORD MakeCheckSum(T data);

private:
	NetManager* m_netMng;
};


template<class T>
DWORD NetPacketProcessor::MakeCheckSum(T data)
{
	DWORD checkSum = 0;
	unsigned char* cursor = (unsigned char*)&data;
	for (int i = 0; i < sizeof(T); i++) {

		checkSum += *cursor;
		cursor++;
	}
	return checkSum;
}