#pragma once
#include "CGameBase.h"

class Animation;

// 캐릭터가 하는 일은?
// 1. 애니메이션을 재생한다.
// 2. 키를 입력받는다.

class Player : public CGameBase
{
public:
	Player();
	virtual ~Player();

public:
	void SetStatus(/*key msg에 따른 캐릭터 상태 정보*/);

	// CGameBase을(를) 통해 상속됨
	virtual bool Initialize(void) override;
	virtual void Release(void) override;
	virtual LONGLONG Update(LONGLONG deltaTime, CScreenDIB * dib, DWORD frameCount) override;
	virtual void Draw(CScreenDIB * dib) override;

	Animation*			m_animStandLeft;
private:
	ImageProcessor*	m_imgProcessor;
	Animation*			m_animStandRight;
	Animation*			m_animMoveLeft;
	Animation*			m_animMoveRight;
	Animation*			m_animAttackZapLeft;
	Animation*			m_animAttackZapRight;
	Animation*			m_animPaunchLeft;
	Animation*			m_animPaunchRight;
	Animation*			m_animKickLeft;
	Animation*			m_animKickRight;
};