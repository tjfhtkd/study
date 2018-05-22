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
	lineBuf = nullptr;
	return target;
}

// 손 좀 많이 봐야할 듯
void ImageProcessor::Clipping(OUT AnimStruct* target, IN Position* targetPos, IN RECT clippingArea, OUT CScreenDIB* dibBuf, IN PIXEL colorKey)
{
	PIXEL* buf = (PIXEL*)((void*)dibBuf->GetDibBuffer());
	LONG nextLineTerm = dibBuf->GetPitch() / sizeof(PIXEL);
	LONG height = target->sprite->bmpInfoHeader->bmiHeader.biHeight;
	LONG width = target->sprite->pitch / sizeof(PIXEL);
	SHORT posX = (targetPos != nullptr) ? targetPos->X : 0;
	SHORT posY = (targetPos != nullptr) ? targetPos->Y : 0;

	int correctionLeft = 0;
	if (posX < clippingArea.left)
	{
		correctionLeft = abs(posX);
		if (correctionLeft > width)
		{
			correctionLeft = width;
		}
	}

	int correctionRight = 0;
	if (posX + width > clippingArea.right)
	{
		correctionRight = (posX + width) - clippingArea.right;
		if (correctionRight >= clippingArea.right + width)
		{
			correctionRight = clippingArea.right;
		}
	}

	int correctionTop = 0;
	if (posY < clippingArea.top)
	{
		correctionTop = abs(posY);
		if (correctionTop > height)
		{
			correctionTop = height;
		}
	}

	int correctionBotton = 0;
	if (posY + height > clippingArea.bottom)
	{
		correctionBotton = (posY + height) - clippingArea.bottom;
		if (correctionBotton >= clippingArea.bottom + height)
		{
			correctionBotton = clippingArea.bottom;
		}
	}

	buf += posX;
	if (posY > 0)
	{
		buf += clippingArea.right * posY;
	}

	PIXEL* cursorCharacterframe = (PIXEL*)((void*)target->sprite->data);
	cursorCharacterframe += correctionTop * width;
	cursorCharacterframe += correctionLeft;
	for (int i = correctionTop; i < height - correctionBotton; i++)
	{
		for (int j = correctionLeft; j < width - correctionRight; j++)
		{
			if (cursorCharacterframe->element.red != colorKey.element.red &&
				cursorCharacterframe->element.green != colorKey.element.green &&
				cursorCharacterframe->element.blue != colorKey.element.blue)
			{
				*(buf + j) = *cursorCharacterframe;
			}
			cursorCharacterframe++;
		}
		buf += nextLineTerm;
		cursorCharacterframe += correctionLeft + correctionRight;
	}
}

bool ImageProcessor::AlphaBlending(OUT AnimStruct* target, IN Position blendingStartPos, IN RECT* blendingArea, IN PIXEL colorKey, OUT CScreenDIB* dibBuf)
{
	if (dibBuf == nullptr || target == nullptr)
	{
		return false;
	}

	int overSizeW = dibBuf->GetWidth() - blendingStartPos.X;
	if (overSizeW > 0)
	{
		// x값이 양수일 때
		if (overSizeW < blendingArea->right)
		{
			blendingArea->right = overSizeW;
		}

		// x값이 음수일 때
		if (overSizeW > dibBuf->GetWidth())
		{
			if (overSizeW > dibBuf->GetWidth() + blendingArea->right)
			{
				// 화면 밖을 좌측으로 벗어남
				return false;
			}
			blendingArea->left = abs(blendingStartPos.X);
			blendingStartPos.X = 0;
		}
	}
	else
	{
		// 화면 밖을 우측으로 벗어남
		return false;
	}

	int overSizeH = dibBuf->GetHeight() - blendingStartPos.Y;
	if (overSizeH > 0)
	{
		// y값이 양수일 때
		if (overSizeH < blendingArea->bottom)
		{
			blendingArea->bottom = overSizeH;
		}

		// y값이 음수일 때
		if (overSizeH > dibBuf->GetHeight())
		{
			if (overSizeH > dibBuf->GetHeight() + blendingArea->bottom)
			{
				// 화면 밖을 위로 벗어남
				return false;
			}
			blendingArea->top = overSizeH - dibBuf->GetHeight();
			blendingStartPos.Y = 0;
		}
	}
	else
	{
		// 화면 밖을 아래로 벗어남
		return false;
	}

	PIXEL* obj = target->sprite->data;
	PIXEL* buf = (PIXEL*)dibBuf->GetDibBuffer();
	buf += blendingStartPos.X;
	buf += blendingStartPos.Y * (dibBuf->GetPitch() / sizeof(PIXEL));
	for (int i = blendingArea->top; i < blendingArea->bottom; i++)
	{
		for (int j = blendingArea->left; j < blendingArea->right; j++)
		{
			if (j > target->sprite->pitch/ sizeof(PIXEL))
			{
				break;
			}
			if (colorKey.data & 0x00ffffff == (obj + j)->data)
			{
				continue;
			}
			(obj + j)->element.alpha	= 0xff;
			(obj + j)->element.red		= ((buf + j)->element.red / 2) + (((obj + j))->element.red / 2);
			(obj + j)->element.green	= ((buf + j)->element.green / 2) + ((obj + j)->element.green / 2);
			(obj + j)->element.blue		= ((buf + j)->element.blue / 2) + ((obj + j)->element.blue / 2);
		}
		if (i >= target->sprite->bmpInfoHeader->bmiHeader.biHeight - 1)
		{
			break;
		}
		obj += target->sprite->pitch / sizeof(PIXEL);
		buf += dibBuf->GetPitch() / sizeof(PIXEL);
	}

	return true;
}