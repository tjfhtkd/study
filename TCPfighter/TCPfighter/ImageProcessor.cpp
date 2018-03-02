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
