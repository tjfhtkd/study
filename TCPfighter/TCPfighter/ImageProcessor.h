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
	// OUT ������ ���ʿ� ����, IN ������ ���ʿ� ���Ƶδ°�..?
	// �� ��ũ�ζ� �Լ� ȣ���ϴ� ����� ������ �����µ�, ���� ���� �̸� ��ü�� out_ ���̴� �� ���� ��. in�� ������ out ������ in�̰�.
	void Clipping(OUT AnimStruct* target, IN Position* targetPos, IN RECT clippingArea, OUT CScreenDIB* dibBuf, IN PIXEL colorKey);
	bool AlphaBlending(OUT AnimStruct* target, IN Position* blendingStartPos, IN RECT* blendingArea, IN PIXEL colorKey, OUT CScreenDIB* dibBuf);
};

