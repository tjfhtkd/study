#pragma once
#include "CGameBase.h"

class Animation
{
public:
	Animation(INT playImgAmount);
	~Animation();

public:
	AnimStruct* MakeAnimationStruct(KsDIB * image, INT frameDelay, COORD centerPos);
	bool AddSprite(AnimStruct* sprite, bool bOverride = false);
	bool AddSprite(AnimStruct* sprite, INT idx, bool bOverride = false);
	/*
	* @brief : 애니메이션 이미지를 쓰고자 할 때, 이미지 순서를 delay에 맞게 조정한다.
	* @param frameCount : update frame count 값.
	*/
	void Play(INT frameCount);
	/*
	* @brief : 이미지를 기본 이미지로 되돌린다.
	*/
	void Stop(void);
	/*
	* @brief : 이미지를 일시정지 시킨다. 프레임이 아무리 지나도 애니메이션이 재생되진 않는다.
	*/
	void Pause(void);
	/*
	* @brief : Pause 상태를 해제한다.
	*/
	void Replay(void);

	AnimStruct* GetCurrentSprite(void);

private:
	INT	m_currSpriteIdx;
	INT	m_bPause;
	INT	m_AnimReversePlay;	// 1 또는 -1 값을 지닌다. 초기값이 -1이 되어야 +1씩 한다.
	// 재생할 이미지를 기록한 저장소.
	// 용량 절약을 위해 Vector를 선택함.
	std::vector<AnimStruct*>	m_anim;
};

