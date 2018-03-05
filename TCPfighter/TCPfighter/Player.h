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
	enum class Status {
		MOVE_UP, MOVE_UPLEFT, MOVE_UPRIGHT
		, MOVE_DOWN, MOVE_DOWN_LEFT, MOVE_DOWN_RIGHT
		, STAND_R, STAND_L
		, MOVE_R, MOVE_L
		, ATTACK_ZAP_R, ATTACK_ZAP_L
		, ATTACK_PAUNCH_R, ATTACK_PAUNCH_L
		, ATTACK_KICK_R, ATTACK_KICK_L
		, STATUS_END_SIZE
	};

public:
	void SetStatus(Status status);
	Player::Status GetStatus(void);
	bool AddAnimations(Animation* anim, Status status);
	Animation* GetAnimation(Status status);

	// Plyaer behaviour
	void Move(Status moveStatus);
	void Attack(Status attackStatus);

	// CGameBase을(를) 통해 상속됨
	virtual bool Initialize(void) override;
	virtual void Release(void) override;
	virtual void KeyProcess(KeyMsg keyMsg) override;
	virtual LONGLONG Update(LONGLONG deltaTime, CScreenDIB * dib, DWORD frameCount) override;
	virtual void Draw(CScreenDIB * dib) override;

public:
	COORD	Position;

private:
	Animation**			animations;
	ImageProcessor*	m_imgProcessor;
	Status					m_currentStatus;
};