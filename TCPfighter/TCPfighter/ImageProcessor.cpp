#include "stdafx.h"
#include "ImageProcessor.h"


ImageProcessor::ImageProcessor()
{
}


ImageProcessor::~ImageProcessor()
{
}


KsDIB* ImageProcessor::TranslateBmp(BMP* transTarget)
{
	/*KsDIB* ksDib = (KsDIB*)malloc(sizeof(KsDIB));
	ksDib->bmpInfoHeader = transTarget->bmpInfoHeader;
	ksDib->data = transTarget->data;*/
	BMP* tmp = transTarget;
	BYTE* stamp = (BYTE*) tmp;
	stamp += sizeof(transTarget->bmpFileHeader);
	KsDIB* ksDib = (KsDIB*) stamp;
	return ksDib;
}

KsDIB* ImageProcessor::ReverseBmpData(BMP* target)
{
	if (target != nullptr)
	{
		return ReverseBmpData(TranslateBmp(target));
	}
	return nullptr;
}

KsDIB* ImageProcessor::ReverseBmpData(KsDIB* target)
{
	int height			= target->bmpInfoHeader->bmiHeader.biHeight;
	LONG pitch		= target->pitch;
	BYTE* lineBuf	= (BYTE*)malloc(pitch);

	// swap 하고 동일한 로직.
	BYTE* cursor			= (BYTE*)target->data;
	int loopCnt			= height / 2;
	for (int i = 0; i < loopCnt; i++)
	{
		// tmp = buf[first + i]
		memcpy_s(lineBuf, pitch, cursor + (i * pitch), pitch);
		// buf[first + i] = buf[last - i]
		memcpy_s(cursor + (i * pitch), pitch, cursor + ((height - (i + 1)) * pitch), pitch);
		// buf[last - i] = tmp
		memcpy_s(cursor + ((height - (i + 1)) * pitch), pitch, lineBuf, pitch);
	}
	free(lineBuf);
	return target;
}

// 손 좀 많이 봐야할 듯
void ImageProcessor::Clipping(OUT AnimStruct* target, IN RECT clippingArea, OUT BYTE* dibBuf, DWORD colorKey)
{
	PIXEL* buf = (PIXEL*)((void*)dibBuf);
	LONG height = target->sprite->bmpInfoHeader->bmiHeader.biHeight;
	LONG width = target->sprite->pitch / sizeof(PIXEL);
	SHORT posX = target->centerPos.X;
	SHORT posY = target->centerPos.Y;
	//LONG clippingAreaPitch = 

	int correctionLeft = 0;
	if (posX < clippingArea.left)
	{
		correctionLeft = abs(posX);
		if (correctionLeft > width)
		{
			correctionLeft = width - 1;
		}
	}

	int correctionRight = 0;
	if (posX + width > clippingArea.right)
	{
		correctionRight = (posX + width) - clippingArea.right;
		if (correctionRight >= clippingArea.right + width)
		{
			correctionRight = clippingArea.right - 1;
		}
	}

	int correctionTop = 0;
	if (posY < clippingArea.top)
	{
		correctionTop = abs(posY);
		if (correctionTop > height)
		{
			correctionTop = height - 1;
		}
	}

	int correctionBotton = 0;
	if (posY + height > clippingArea.bottom)
	{
		correctionBotton = (posY + height) - clippingArea.bottom;
		if (correctionBotton >= clippingArea.bottom + height)
		{
			correctionBotton = clippingArea.bottom - 1;
		}
	}

	buf += posX;
	if (posY > 0) {
		buf += clippingArea.right * posY;
	}

	PIXEL* cursorCharacterframe = (PIXEL*)((void*)target->sprite->data);
	cursorCharacterframe += correctionTop * width;
	cursorCharacterframe += correctionLeft;
	for (int i = correctionTop; i < height - correctionBotton; i++)
	{
		for (int j = correctionLeft; j < width - correctionRight; j++)
		{
			if (cursorCharacterframe->data != colorKey)
			{
				*(buf + j) = *cursorCharacterframe;
			}
			cursorCharacterframe++;
		}
		buf += clippingArea.right;
		cursorCharacterframe += correctionLeft + correctionRight;
	}
}