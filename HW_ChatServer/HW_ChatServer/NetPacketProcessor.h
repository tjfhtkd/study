#pragma once

class NetManager;

class NetPacketProcessor
{
public:
	NetPacketProcessor(NetManager* netMng);
	virtual ~NetPacketProcessor();

public:
	void ProcessSessionRequest(std::list<kks::Session*>& rSessions, std::list<kks::Session*>& wSessions);
	
	// 로그인
	//bool RequestLogIn();		// 딱히 할게 없음
	void ResponseLogIn(kks::Session& session);

	// 대화방 목록
	bool RequestRoomList();
	bool ResponseRoomList();

	// 대화방 생성
	bool RequestRoomCreation();
	bool ResponseRoomCreation();

	// 대화방 삭제
	bool ResponseRoomDelete();

	// 대화방 입장
	bool RequestJoinRoom();
	bool ResponseJoinRoom();

	// 대화방 퇴장
	bool RequestExitRoom();
	bool ResponseExitRoom(kks::Session* leavingSession);

	// 채팅 메시지 송신/수신
	bool RequestSendMsg();
	bool ResponseSendMsg();

	// 타 사용자 입장
	bool ResponseUserEnter();

private:
	bool IsIntactPacket(kks::Session& session);
	SOCKET Accept(SOCKET listenSock);
	void AnalysePacket(kks::Session& session);

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