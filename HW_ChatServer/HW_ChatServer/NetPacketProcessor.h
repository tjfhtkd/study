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
	bool RequestLogIn();
	bool ResponseLogIn();

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
	bool ResponseExitRoom();

	// 채팅 메시지 송신/수신
	bool RequestSendMsg();
	bool ResponseSendMsg();

	// 타 사용자 입장
	bool ResponseUserEnter();

private:
	bool CheckHeader(CStreamSQ& buf);
	bool IsNormalityChecksum(st_PACKET_HEADER& header);
	SOCKET Accept(SOCKET listenSock);

private:
	NetManager* m_netMng;
};