#pragma once
class Shadow
{
public:
	Shadow(ImageProcessor* imgProcessor, AnimStruct* shadowImg);
	virtual ~Shadow();

public:
	void ProcAlphaBlending(COORD* alignedShadowPos, RECT* blendingArea, CScreenDIB* backBuf);
	AnimStruct* GetBlendedImg(void);
	AnimStruct* GetNormalImg(void);
	void ChangeShadowSprite(AnimStruct* shadowImg);
	COORD GetCenterPos(void);

public:
	COORD	Position;
	bool		On;

private:
	ImageProcessor*	m_imgProcessor;
	AnimStruct*			m_shadowImg;
	AnimStruct*			m_prevBlendedImg;
};

