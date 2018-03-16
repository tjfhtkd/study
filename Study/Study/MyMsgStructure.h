#pragma once

#pragma pack(1)

// 메타헤더
typedef struct GeneralMsgStruct
{
	// 이 프로젝트에서 해석 가능한 모든 프로토콜의 인터페이스
	BYTE checkCode;	// 프로그램에 맞는 메시지인지 확인하는 용도
	BYTE len[2];			// 이 메시지 이후에 올 버퍼의 크기값을 확인하는 용도
	BYTE checksum;		// 데이터 위변조 방지용 체크섬
	BYTE key;				// XOR 암복호화에 사용되는 key 값
	BYTE seqNum;		// 서버-클라가 서로 주고받는 메시지의 쌍을 확인하기 위한 값
	BYTE type;			// 헤더 종류 구분용
	ULONGLONG		senderID;
	ULONGLONG		receiverID;
} stGeneralFrame;


// 채팅헤더
typedef struct ChattingMsgStruct
{
	INT				roomNum;
} stChatFrame;


// 헤더 생성/분석용
typedef struct GeneralHeaderFrame
{
	stGeneralFrame generalFrame;
	union MsgFrame
	{
		stChatFrame chatHeader;
		// 이후 새로운 헤더 추가할 때마다 여기에 저장.
	};
	MsgFrame supplementFrame;
} stGeneralStamp;

#pragma pack()