#include "stdafx.h"
#include "Shadow.h"


Shadow::Shadow(ImageProcessor* imgProcessor, AnimStruct* shadowImg)
{
	m_imgProcessor = imgProcessor;
	m_shadowImg = shadowImg;
	m_prevBlendedImg = nullptr;
	On = true;
}

Shadow::~Shadow()
{
	// 여기저기서 얘를 쓰고 지우려고 할 때...
	// 누가 지우건 신경쓰지 않겠다. 안지워졌으면 내가 마저 지우겠다 형태로 해봄.
	// 단점 - 난잡해짐.
	// 장점 - 지우는건 확실해짐.
	if (m_prevBlendedImg != nullptr)
	{
		if (m_prevBlendedImg->sprite != nullptr)
		{
			if (m_prevBlendedImg->sprite->bmpInfoHeader != nullptr)
			{
				free(m_prevBlendedImg->sprite->bmpInfoHeader);
				m_prevBlendedImg->sprite->bmpInfoHeader = nullptr;
			}

			if (m_prevBlendedImg->sprite->data != nullptr)
			{
				free(m_prevBlendedImg->sprite->data);
				m_prevBlendedImg->sprite->data = nullptr;
			}
		}

		free(m_prevBlendedImg->sprite);
		m_prevBlendedImg->sprite = nullptr;
	}

	if (m_prevBlendedImg != nullptr)
	{
		free(m_prevBlendedImg);
		m_prevBlendedImg = nullptr;
	}
	m_imgProcessor = nullptr;
	m_shadowImg = nullptr;
	Position = { 0, 0 };
	On = false;
}

void Shadow::ProcAlphaBlending(COORD* alignedShadowPos, RECT* blendingArea, CScreenDIB * backBuf)
{
	// 생성자에서 아예 1개 만들어두고 재사용 하게... 근본적으론 AnimStruct가 class가 되어야 함.
	if (m_prevBlendedImg != nullptr)
	{
		// 이 사태를 우찌 해야하는가...
		free(m_prevBlendedImg->sprite->bmpInfoHeader);
		m_prevBlendedImg->sprite->bmpInfoHeader = nullptr;

		free(m_prevBlendedImg->sprite->data);
		m_prevBlendedImg->sprite->data = nullptr;

		free(m_prevBlendedImg->sprite);
		m_prevBlendedImg->sprite = nullptr;

		free(m_prevBlendedImg);
		m_prevBlendedImg = nullptr;
	}

	m_prevBlendedImg = (AnimStruct*)malloc(sizeof(AnimStruct));
	m_prevBlendedImg->frameDelay = m_shadowImg->frameDelay;
	m_prevBlendedImg->centerPos = m_shadowImg->centerPos;
	m_prevBlendedImg->colorKey = m_shadowImg->colorKey;

	m_prevBlendedImg->sprite = (KsDIB*)malloc(sizeof(KsDIB));
	m_prevBlendedImg->sprite->bmpInfoHeader = (BITMAPINFO*)malloc(sizeof(BITMAPINFO));

	*m_prevBlendedImg->sprite->bmpInfoHeader = *m_shadowImg->sprite->bmpInfoHeader;
	m_prevBlendedImg->sprite->pitch = m_shadowImg->sprite->pitch;
	
	LONG size = m_shadowImg->sprite->pitch * m_shadowImg->sprite->bmpInfoHeader->bmiHeader.biHeight;
	m_prevBlendedImg->sprite->data = (PIXEL*)malloc(size);
	memcpy_s(m_prevBlendedImg->sprite->data, size, m_shadowImg->sprite->data, size);

	m_imgProcessor->AlphaBlending(m_prevBlendedImg, alignedShadowPos, blendingArea, m_shadowImg->colorKey ,backBuf);
}

AnimStruct* Shadow::GetBlendedImg(void)
{
	return m_prevBlendedImg;
}

AnimStruct* Shadow::GetNormalImg(void)
{
	return m_shadowImg;
}

void Shadow::ChangeShadowSprite(AnimStruct* shadowImg)
{
	if (m_shadowImg != nullptr)
	{
		free(m_shadowImg);
		m_shadowImg = nullptr;
	}
	m_shadowImg = shadowImg;
}

COORD Shadow::GetCenterPos(void)
{
	return m_shadowImg->centerPos;
}