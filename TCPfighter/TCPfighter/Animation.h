#pragma once
#include "CGameBase.h"

class Animation
{
public:
	/*
	* @brief : Animation ��ü�� �������̸� �ִϸ��̼��� �� �� ����� ���ΰ�, ��� ����� ���ΰ��� �����Ѵ�.
	* @param playImgAmount : ����� �̹����� �� ��.
	* @param playCount : �ݺ� ����� Ƚ��. 0�̸� ���� �ݺ�.
	* @param bCircularPlay : ��ȯ ��� �����̸�, 1->2->3->2->1 ���¸� True, 1->2->3->1->2->3 �̸� False �̴�.
	*/
	Animation(INT playImgAmount, bool bCircularPlay);
	virtual ~Animation();

public:
	AnimStruct* MakeAnimationStruct(KsDIB * image, INT frameDelay, Position centerPos);
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
	void Reset(void);
	/*
	* @brief : �̹����� �Ͻ����� ��Ų��. �������� �ƹ��� ������ �ִϸ��̼��� ������� �ʴ´�.
	*/
	void Pause(bool bPause);

	/*
	* @brief : ���� �ִϸ��̼� ������ �������� �ƴ��� ���θ� �˷��ش�.
	* @detail : ���� �ִϸ��̼� ��ǰ��� �������� ��ȯ���� �����Ǹ�, Pause�� �Ǿ�����
	              ��ǰ��� �����Ƿ� Pause ���οʹ� �������.
	* @return : �ִϸ��̼� ������ ���� ���¸� True, ���� �ִϸ��̼��� ������̸� False.
	*/
	bool IsEnd(void);

	AnimStruct* GetCurrentSprite(void);

private:
	INT	m_frameDelayCount;	// ����� �ִϸ��̼� ����� ���� �����.
	bool	m_bEnd;					// �ִϸ��̼��� ��������� �ƴ��� �Ǵ��ϱ� ���� �÷���.

	bool	m_bCircularPlay;
	INT	m_currSpriteIdx;
	INT	m_bPause;
	INT	m_AnimReversePlay;	// 1 �Ǵ� -1 ���� ���Ѵ�. �ʱⰪ�� -1�� �Ǿ�� +1�� �Ѵ�.
	// ����� �̹����� ����� �����.
	// �뷮 ������ ���� Vector�� ������.
	std::vector<AnimStruct*>	m_anim;
};