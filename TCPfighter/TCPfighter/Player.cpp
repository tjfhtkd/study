#include "stdafx.h"

#include "Player.h"


Player::Player()
{
}

Player::~Player()
{
}
/////////////////////////////// Public ///////////////////////////////
bool Player::Initialize(void)
{
	m_imgProcessor = new ImageProcessor();
	return true;
}

void Player::Release(void)
{
}

LONGLONG Player::Update(LONGLONG deltaTime, CScreenDIB* dib, DWORD frameCount)
{
	m_animStandLeft->Play(frameCount);
	AnimStruct* animImg = m_animStandLeft->GetCurrentSprite();
	m_imgProcessor->Clipping(animImg, GameSystemInfo::GetInstance()->WindowSize, dib->GetDibBuffer(), 0xffffffff );
	return deltaTime;
}

void Player::Draw(CScreenDIB* dib)
{
}



/////////////////////////////// Private ///////////////////////////////