#pragma once
#include "CGameBase.h"

class Animation
{
public:
	/*
	* @brief : Animation 객체의 생성자이며 애니메이션을 몇 장 재생할 것인가, 어떻게 재생할 것인가를 결정한다.
	* @param playImgAmount : 재생할 이미지의 장 수.
	* @param playCount : 반복 재생할 횟수. 0이면 무한 반복.
	* @param bCircularPlay : 순환 재생 여부이며, 1->2->3->2->1 형태면 True, 1->2->3->1->2->3 이면 False 이다.
	*/
	Animation(INT playImgAmount, bool bCircularPlay);
	virtual ~Animation();

public:
	AnimStruct* MakeAnimationStruct(KsDIB * image, INT frameDelay, Position centerPos);
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
	void Reset(void);
	/*
	* @brief : 이미지를 일시정지 시킨다. 프레임이 아무리 지나도 애니메이션이 재생되진 않는다.
	*/
	void Pause(bool bPause);

	/*
	* @brief : 현재 애니메이션 동작이 끝났는지 아닌지 여부를 알려준다.
	* @detail : 현재 애니메이션 모션값을 기준으로 반환값이 결정되며, Pause가 되었더라도
	              모션값을 따지므로 Pause 여부와는 상관없다.
	* @return : 애니메이션 동작이 끝난 상태면 True, 현재 애니메이션이 재생중이면 False.
	*/
	bool IsEnd(void);

	AnimStruct* GetCurrentSprite(void);

private:
	INT	m_frameDelayCount;	// 재생할 애니메이션 계산을 위해 세어둠.
	bool	m_bEnd;					// 애니메이션이 재생중인지 아닌지 판단하기 위한 플래그.

	bool	m_bCircularPlay;
	INT	m_currSpriteIdx;
	INT	m_bPause;
	INT	m_AnimReversePlay;	// 1 또는 -1 값을 지닌다. 초기값이 -1이 되어야 +1씩 한다.
	// 재생할 이미지를 기록한 저장소.
	// 용량 절약을 위해 Vector를 선택함.
	std::vector<AnimStruct*>	m_anim;
};