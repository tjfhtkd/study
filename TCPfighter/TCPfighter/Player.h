#pragma once
#include "CGameBase.h"

class Animation;

// ĳ���Ͱ� �ϴ� ����?
// 1. �ִϸ��̼��� ����Ѵ�.
// 2. Ű�� �Է¹޴´�.

class Player : public CGameBase
{
public:
	Player();
	virtual ~Player();

public:
	void SetStatus(/*key msg�� ���� ĳ���� ���� ����*/);

	// CGameBase��(��) ���� ��ӵ�
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