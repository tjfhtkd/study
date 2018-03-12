#include "stdafx.h"
#include "Animation.h"


Animation::Animation(INT playImgAmount, bool bCircularPlay)
{
	m_anim.reserve(playImgAmount);
	m_currSpriteIdx = -1;
	m_frameDelayCount = -1;
	m_AnimReversePlay = 1;
	m_bCircularPlay = bCircularPlay;
	m_bEnd = false;
}

Animation::~Animation()
{
	for (int i = 0; i < m_anim.size(); i++)
	{
		free(m_anim[i]);
		m_anim[i] = nullptr;
	}
	m_anim.clear();
}

//////////////////////// Public ////////////////////////

AnimStruct* Animation::MakeAnimationStruct(KsDIB* image, INT frameDelay, COORD centerPos)
{
	AnimStruct* tmp = (AnimStruct*)malloc(sizeof(AnimStruct));
	tmp->sprite = image;
	tmp->frameDelay = frameDelay;
	tmp->centerPos = centerPos;
	return tmp;
}

bool Animation::AddSprite(AnimStruct* sprite, bool bOverride)
{
	return AddSprite(sprite, m_anim.size(), bOverride);
}

bool Animation::AddSprite(AnimStruct* sprite, INT idx, bool bOverride)
{
	if (idx < 0 && idx >= m_anim.capacity())
	{
		return false;
	}

	if (bOverride == false && m_anim.size() >= m_anim.capacity())
	{
		return false;
	}

	if (bOverride)
	{
		AnimStruct* delTarget = m_anim[idx];
		free(delTarget);
		delTarget = nullptr;
		m_anim[idx] = sprite;
	}
	else
	{
		m_anim.push_back(sprite);
	}
	return true;
}

void Animation::Play(INT frameCount)
{
	if(m_bPause || m_bEnd) {
		return;
	}

	// 정방향 애니메이션 재생 1->2->3[end] 1->2->3[end]
	m_frameDelayCount++;
	if (m_frameDelayCount % m_anim[m_currSpriteIdx + 1]->frameDelay == 0)
	{
		m_currSpriteIdx += m_AnimReversePlay;
		m_frameDelayCount = 0;
	}

	if (m_currSpriteIdx == m_anim.size() - 1)
	{
		m_bEnd = true;
		m_frameDelayCount = -1;
	}

	// 역방향 애니메이션 로직으로 변환 1->2->3->2->1[end]
	if (m_currSpriteIdx == -2)
	{
		m_bEnd = true;
		m_currSpriteIdx = -1;
		m_frameDelayCount = -1;
		m_AnimReversePlay = 1;
	}
	else
	{
		if (m_bCircularPlay == true && m_bEnd == true)
		{
			m_bEnd = false;
			m_currSpriteIdx--;
			m_AnimReversePlay = -1;
		}
	}
}

void Animation::Reset(void)
{
	m_bEnd = false;
	m_currSpriteIdx = -1;
	m_frameDelayCount = -1;
	m_AnimReversePlay = 1;
}

void Animation::Pause(bool bPause)
{
	m_bPause = bPause;
}

bool Animation::IsEnd(void)
{
	return m_bEnd;
}

AnimStruct* Animation::GetCurrentSprite(void)
{
	if (m_bCircularPlay)
	{
		return m_anim[m_currSpriteIdx + 1];
	}
	return m_anim[m_currSpriteIdx];
}