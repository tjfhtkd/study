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
	// OUT 변수를 앞쪽에 몰고, IN 변수를 뒤쪽에 몰아두는게..?
	// 빈 매크로라서 함수 호출하는 사람은 어차피 못보는데, 차라리 변수 이름 자체에 out_ 붙이는 게 나을 듯. in은 어차피 out 없으면 in이고.
	void Clipping(OUT AnimStruct* target, IN Position* targetPos, IN RECT clippingArea, OUT CScreenDIB* dibBuf, IN PIXEL colorKey);
	bool AlphaBlending(OUT AnimStruct* target, IN Position* blendingStartPos, IN RECT* blendingArea, IN PIXEL colorKey, OUT CScreenDIB* dibBuf);
};

