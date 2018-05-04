// This chat server use Select Model.

#pragma once

#include "Protocol.h"
#include "CStreamSQ.h"

#define	dfMAX_ROOM_CHATTER		FD_SETSIZE
namespace kks
{
	typedef CStreamSQ RingBuf;

	struct Session
	{
		DWORD	uid;
		DWORD	roomNo;
		SOCKET	sock;
		WCHAR	nickName[dfNICK_MAX_LEN];
		RingBuf	recvQ;
		RingBuf	sendQ;
	};

	struct Room
	{
		INT		chatterCnt;
		INT		maxChatterCnt;
		DWORD	roomNo;
		INT		chatterMaxCnt;
		INT		chatterJoinCnt;
		INT		uidList[dfMAX_ROOM_CHATTER];
	};
}