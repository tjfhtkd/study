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
	* @brief : �ִϸ��̼� �̹����� ������ �� ��, �̹��� ������ delay�� �°� �����Ѵ�.
	* @param frameCount : update frame count ��.
	*/
	void Play(INT frameCount);
	/*
	* @brief : �̹����� �⺻ �̹����� �ǵ�����.
	*/
	void Stop(void);
	/*
	* @brief : �̹����� �Ͻ����� ��Ų��. �������� �ƹ��� ������ �ִϸ��̼��� ������� �ʴ´�.
	*/
	void Pause(void);
	/*
	* @brief : Pause ���¸� �����Ѵ�.
	*/
	void Replay(void);

	AnimStruct* GetCurrentSprite(void);

private:
	INT	m_currSpriteIdx;
	INT	m_bPause;
	INT	m_AnimReversePlay;	// 1 �Ǵ� -1 ���� ���Ѵ�. �ʱⰪ�� -1�� �Ǿ�� +1�� �Ѵ�.
	// ����� �̹����� ����� �����.
	// �뷮 ������ ���� Vector�� ������.
	std::vector<AnimStruct*>	m_anim;
};

