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