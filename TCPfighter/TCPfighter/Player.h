#pragma once
#include "CGameBase.h"

class Animation;
class HpGuage;
class Shadow;
class NetMessenger;

class Player : public CGameBase
{
public:
	Player(HpGuage* hpGuage, ImageProcessor* imageProcessor);
	virtual ~Player();

public:
	enum class Status {
		MOVE_L, MOVE_UPLEFT, MOVE_UP, MOVE_UPRIGHT
		, MOVE_R, MOVE_DOWN_RIGHT
		, MOVE_DOWN, MOVE_DOWN_LEFT
		, STAND_R, STAND_L
		, ATTACK_ZAP_R, ATTACK_ZAP_L
		, ATTACK_PAUNCH_R, ATTACK_PAUNCH_L
		, ATTACK_KICK_R, ATTACK_KICK_L
		, END_SIZE
	};

protected:
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
	void SetMessenger(NetMessenger* messenger);
	void SendActionPacket(int overrappedKeyMsg, Status prevStatus, Status currStatus);
	void AssemblePacket(int packetType, Status currStatus);

	// Plyaer behaviour
	void Move(Status moveStatus);
	void Attack(Status attackStatus);


	// CGameBase을(를) 통해 상속됨
	virtual bool Initialize(void) override;
	virtual void Release(void) override;
	virtual void KeyProcess(KeyMsg keyMsg) override;
	virtual LONGLONG Update(LONGLONG deltaTime, CScreenDIB * dib, DWORD frameCount) override;
	virtual void Draw(CScreenDIB * dib) override;
	virtual void CommunicateNetwork(stPacket_ArgCollectionBox intendBox) override;

protected:
	int MakeOverrapedKeyMsg(void);
	LookDirection GetLookDirection(int overrappedKeyMsg, LookDirection prevDirection);
	void DeterminePlayAnimation(int overrappedKeyMsg, LookDirection currentDirection);
	void CheckOutOfPlayArea(Position& position);

public:
	HpGuage*	Hp;

protected:
	bool					m_bAttack;
	INT					m_effectStartFrameNum;
	Animation**			animations;
	// 제대로 한다면 나중에 이펙트 클래스 따로 뽑아두기. 어차피 정렬해야해서 따로 뽑아야 함.
	Animation*			m_effect;
	Position				m_effectStartPos;

	ImageProcessor*	m_imgProcessor;
	Shadow*				m_shadow;
	Status					m_currentStatus;
	Status					m_prevStatus;
	LookDirection		m_prevDirection;

	NetMessenger*		m_messenger;
};