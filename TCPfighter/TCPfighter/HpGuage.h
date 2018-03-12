#pragma once
class HpGuage : public CGameBase
{
public:
	HpGuage(ImageProcessor* imgProcessor, AnimStruct* hpImg, INT maxHP = 100);
	~HpGuage();

public:
	bool GetDamaged(INT damage);
	INT GeCurrentHP(void);
	INT GeMaxHP(void);
	void SetMaxHP(INT changedMaxHP);
	bool IsDead(void);
	void ChangeHpSprite(AnimStruct* hpImg);
	COORD GetCenterPos(void);
	
	// CGameBase을(를) 통해 상속됨
	virtual bool Initialize(void) override;
	virtual void Release(void) override;
	virtual LONGLONG Update(LONGLONG deltaTime, CScreenDIB* dib, DWORD frameCount) override;
	virtual void Draw(CScreenDIB * dib) override;

public:
	COORD	Position;

private:
	INT					m_maxHP;
	INT					m_currHP;
	AnimStruct*			m_HpGuageInfo;
	ImageProcessor*	m_imgProcessor;
	CScreenDIB*			m_hpBarBackBuf;
};

