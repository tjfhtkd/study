#pragma once

class ImageProcessor
{
public:
	ImageProcessor();
	~ImageProcessor();

public:
	KsDIB* TranslateBmp(BMP* transTarget);
	KsDIB* ReverseBmpData(BMP* target);
	KsDIB* ReverseBmpData(KsDIB* target);
	void Clipping(OUT AnimStruct* target, IN COORD* targetPos, IN RECT clippingArea, OUT BYTE* dibBuf, DWORD colorKey);
};

