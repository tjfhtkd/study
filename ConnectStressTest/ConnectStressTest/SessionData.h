#pragma once

typedef struct Session
{
	SOCKET	sock;
	INT		id;
	DWORD	sendTime;				// ������ ���´� ���� �ð�
	WCHAR	sendEchoMsg[128];		// ������ ���´� ���� �޽���, ������ �� �ְ�޾Ҵ��� Ȯ�ο�
	bool		bRecvWait;				// ���ڸ� ������ ���� ACK ��� ���� ���� ǥ��
	RingBuf	sendQ;
	RingBuf	recvQ;
} Client;