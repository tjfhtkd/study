#pragma once

// �̹����� ������ �Ǿ����� �ʴٴ� ���� �Ͽ� �������. ( biCompress != BI_RGB )

typedef union BMPBitStamp {
	DWORD	data;
	struct ELEMENT {
		BYTE blue;
		BYTE green;
		BYTE red;
		BYTE alpha;
	};
	ELEMENT element;
} PIXEL;

typedef struct BMPExtractedData {
	// BMP ���� ���, DDB�� ��ȯ�ϱ� ���� �ʿ�
	BITMAPINFO* bmpInfoHeader;
	// ���� �̹��� ������
	PIXEL* data;
	// ��ġ��
	LONG pitch;
} KsDIB;

typedef struct Bitmap {
	BITMAPFILEHEADER	*	bmpFileHeader;
	BITMAPINFO*			bmpInfoHeader;
	PIXEL*						data;
	LONG						pitch;		// KsDIB Use Only	->malloc �Ҵ� ��Ŀ��� ��Ʈ ���� ���.
} BMP;

/*
* @breif : �Է¹��� Ư�� Ű ���� ���ӿ��� �״�� �̿����� �ʴ´�.
* Ư�� Ű ���� KeyMsg�� �°� ��Ī�Ͽ� ����ؾ� �Ѵ�.
*/
enum class KeyMsg : UINT {
	// DEFAULT
	DO_NOTHING	= 0
	// MOVE
	, UP				= 1
	, DOWN			= 2
	, LEFT				= 4
	, RIGHT			= 8
	// ATTACK
	, ATK_ZAP		= 16
	, ATK_PAUNCH	= 32
	, ATK_KICK		= 64
	// SIZE
	, KEY_MSG_SIZ = 7

};
//KeyMsg operator | (KeyMsg lhs, KeyMsg rhs);
//KeyMsg& operator |= (KeyMsg& lhs, KeyMsg& rhs);

typedef struct AnimationStruct {
	KsDIB*	sprite;
	INT		frameDelay;
	COORD	centerPos;
	PIXEL		colorKey;
} AnimStruct;