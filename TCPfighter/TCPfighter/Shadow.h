#pragma once

#include "DataStructures.h"

class Shadow
{
public:
	Shadow(ImageProcessor* imgProcessor, AnimStruct* shadowImg);
	virtual ~Shadow();

public:
	void ProcAlphaBlending(Position* alignedShadowPos, RECT* blendingArea, CScreenDIB* backBuf);
	AnimStruct* GetBlendedImg(void);
	AnimStruct* GetNormalImg(void);
	void ChangeShadowSprite(AnimStruct* shadowImg);
	Position GetCenterPos(void);

public:
	Position	position;
	bool		On;

private:
	ImageProcessor*	m_imgProcessor;
	AnimStruct*			m_shadowImg;
	AnimStruct*			m_prevBlendedImg;
};

