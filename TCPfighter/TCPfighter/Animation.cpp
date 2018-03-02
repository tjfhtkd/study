#include "stdafx.h"
#include "Animation.h"


Animation::Animation(INT playImgAmount)
{
	m_anim.reserve(playImgAmount);
	m_currSpriteIdx = 0;
	m_AnimReversePlay = -1;
}

Animation::~Animation()
{
	for (int i = 0; i < m_anim.size(); i++)
	{
		free(m_anim[i]);
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

	if (m_anim[idx] != nullptr)
	{
		free(m_anim[idx]);
	}
	m_anim[idx] = sprite;
	return true;
}

void Animation::Play(INT frameCount)
{
	if(m_bPause) {
		return;
	}
	// puase랑은 상관없이 frameCount는 계속 흐르기 때문에 Replay 할 때
	// 바로 재생 할 때도 있고, 이동은 되는데 애니메이션이 바로 재생이 안 될 때도 있을 것이다.
	// 허나 중요치 않고 귀찮으니 그냥 넘긴다!
	// 제대로 동작시키려면 frameCount를 따로 저장하고 그 값으로 계산.
	if (frameCount % m_anim[m_currSpriteIdx]->frameDelay != 0)
	{
		return;
	}

	if (m_currSpriteIdx == 0 ||
		m_currSpriteIdx == m_anim.size() - 1)
	{
		m_AnimReversePlay *= (-1);
	}
	m_currSpriteIdx += m_AnimReversePlay;
}

void Animation::Stop(void)
{
	m_currSpriteIdx = 0;
}

void Animation::Pause(void)
{
	m_bPause = true;
}

void Animation::Replay(void)
{
	m_bPause = false;
}

AnimStruct* Animation::GetCurrentSprite(void)
{
	return m_anim[m_currSpriteIdx];
}