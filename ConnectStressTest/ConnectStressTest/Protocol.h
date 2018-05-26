#pragma once

#define dfPACKET_CODE		0x89

//------------------------------------------------------
//  패킷헤더
//
//	| PacketCode | sin_zero | MsgType | PayloadSize | * Payload * |
//		1Byte         1Byte       2Byte        2Byte        Size Byte     
//
//------------------------------------------------------

#pragma pack(push, 1)   

struct st_PACKET_HEADER
{
	BYTE	byCode;
	BYTE	sin_zero;				// 테스트용 서버랑 헤더 맞추려고 넣었을 뿐.

	WORD		wMsgType;
	WORD		wPayloadSize;
};

#pragma pack(pop)

#define df_REQ_ECHO	15