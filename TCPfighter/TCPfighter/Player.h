#pragma once
#include "CGameBase.h"

class Animation;
class HpGuage;
class Shadow;

class Player : public CGameBase
{
public:
	Player(HpGuage* hpGuage, ImageProcessor* imageProcessor);
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
		, END_SIZE
	};

private:
	enum class LookDirection {
		LEFT_MOVE, RIGHT_MOVE, LEFT_STAND, RIGHT_STAND
		, END_SIZE
	};

public:
	void SetStatus(Status status);
	Player::Status GetStatus(void);
	bool AddAnimations(Animation* anim, Status status);
	Animation* GetAnimation(Status status);
	void SetShadow(Shadow* shadow);
	Shadow* GetShadow(void);
	void SetEffect(Animation* effectAnim);

	// Plyaer behaviour
	void Move(Status moveStatus);
	void Attack(Status attackStatus);

	// CGameBase��(��) ���� ��ӵ�
	virtual bool Initialize(void) override;
	virtual void Release(void) override;
	virtual void KeyProcess(KeyMsg keyMsg) override;
	virtual LONGLONG Update(LONGLONG deltaTime, CScreenDIB * dib, DWORD frameCount) override;
	virtual void Draw(CScreenDIB * dib) override;

private:
	int MakeOverrapedKeyMsg(void);
	LookDirection GetLookDirection(int overrappedKeyMsg, LookDirection prevDirection);
	void DeterminePlayAnimation(int overrappedKeyMsg, LookDirection currentDirection);
	void CheckOutOfPlayArea(COORD& position);

public:
	COORD		Position;
	HpGuage*	Hp;

private:
	bool					m_bAttack;
	INT					m_effectStartFrameNum;
	Animation**			animations;
	// ����� �Ѵٸ� ���߿� ����Ʈ Ŭ���� ���� �̾Ƶα�. ������ �����ؾ��ؼ� ���� �̾ƾ� ��.
	Animation*			m_effect;
	COORD				m_effectStartPos;
	// �������.
	ImageProcessor*	m_imgProcessor;
	Shadow*				m_shadow;
	Status					m_currentStatus;
	LookDirection		m_prevDirection;
};