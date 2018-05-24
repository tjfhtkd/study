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
	//bool RequestLogIn();		// ���� �Ұ� ����
	void ResponseLogIn(kks::Session& session);

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

	void EchoTest(kks::Session& session);

private:
	bool IsIntactPacket(kks::Session& session);
	SOCKET Accept(SOCKET listenSock);
	void AnalysePacket(kks::Session& session);

	template<class T>
	DWORD MakeCheckSum(T data);
	DWORD MakeCheckSum(DWORD in_checkSum, kks::LocalBuf& buf, int size);

private:
	NetManager* m_netMng;
};


template<class T>
DWORD NetPacketProcessor::MakeCheckSum(T data)
{
	DWORD checkSum = 0;
	int size = sizeof(T);
	unsigned char* cursor = (unsigned char*)&data;
	for (int i = 0; i < size; i++) {

		checkSum += *cursor;
		cursor++;
	}
	return checkSum;
}