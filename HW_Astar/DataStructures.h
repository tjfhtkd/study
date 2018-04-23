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

typedef struct AstarNode
{
	/*FLOAT X;
	FLOAT Y;*/
	INT G;
	INT H;
	INT F;
}Node;

typedef struct GridElement
{
	GridElement* parent;
	Node mapNode;
	INT idxX;
	INT idxY;
	PIXEL color;
} MapData;