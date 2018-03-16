#pragma once

#pragma pack(1)

// ��Ÿ���
typedef struct GeneralMsgStruct
{
	// �� ������Ʈ���� �ؼ� ������ ��� ���������� �������̽�
	BYTE checkCode;	// ���α׷��� �´� �޽������� Ȯ���ϴ� �뵵
	BYTE len[2];			// �� �޽��� ���Ŀ� �� ������ ũ�Ⱚ�� Ȯ���ϴ� �뵵
	BYTE checksum;		// ������ ������ ������ üũ��
	BYTE key;				// XOR �Ϻ�ȣȭ�� ���Ǵ� key ��
	BYTE seqNum;		// ����-Ŭ�� ���� �ְ�޴� �޽����� ���� Ȯ���ϱ� ���� ��
	BYTE type;			// ��� ���� ���п�
	ULONGLONG		senderID;
	ULONGLONG		receiverID;
} stGeneralFrame;


// ä�����
typedef struct ChattingMsgStruct
{
	INT				roomNum;
} stChatFrame;


// ��� ����/�м���
typedef struct GeneralHeaderFrame
{
	stGeneralFrame generalFrame;
	union MsgFrame
	{
		stChatFrame chatHeader;
		// ���� ���ο� ��� �߰��� ������ ���⿡ ����.
	};
	MsgFrame supplementFrame;
} stGeneralStamp;

#pragma pack()