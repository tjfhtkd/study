#pragma once

typedef struct Session
{
	SOCKET	sock;
	INT		id;
	DWORD	sendTime;				// 이전에 보냈던 에코 시간
	WCHAR	sendEchoMsg[128];		// 이전에 보냈던 에코 메시지, 서버와 잘 주고받았는지 확인용
	bool		bRecvWait;				// 에코를 보내고 나서 ACK 대기 중인 세션 표시
	RingBuf	sendQ;
	RingBuf	recvQ;
} Client;