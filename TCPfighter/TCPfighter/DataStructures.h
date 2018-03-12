#pragma once

// 이미지가 압축이 되어있지 않다는 가정 하에 만들어짐. ( biCompress != BI_RGB )

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
	// BMP 정보 헤더, DDB로 변환하기 위해 필요
	BITMAPINFO* bmpInfoHeader;
	// 실제 이미지 데이터
	PIXEL* data;
	// 피치값
	LONG pitch;
} KsDIB;

typedef struct Bitmap {
	BITMAPFILEHEADER	*	bmpFileHeader;
	BITMAPINFO*			bmpInfoHeader;
	PIXEL*						data;
	LONG						pitch;		// KsDIB Use Only	->malloc 할당 방식에서 힌트 얻은 기법.
} BMP;

/*
* @breif : 입력받은 특정 키 값을 게임에서 그대로 이용하지 않는다.
* 특정 키 값을 KeyMsg에 맞게 매칭하여 사용해야 한다.
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