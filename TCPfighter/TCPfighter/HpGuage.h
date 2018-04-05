#pragma once
class HpGuage : public CGameBase
{
public:
	HpGuage(ImageProcessor* imgProcessor, AnimStruct* hpImg, INT maxHP = 100);
	virtual ~HpGuage();

public:
	bool GetDamaged(INT damage);
	INT GetCurrentHP(void);
	INT GetMaxHP(void);
	void SetMaxHP(INT changedMaxHP);
	void SetCurrHP(INT changedCurrentHP);
	bool IsDead(void);
	void ChangeHpSprite(AnimStruct* hpImg);
	Position GetCenterPos(void);
	
	// CGameBase을(를) 통해 상속됨
	virtual bool Initialize(void) override;
	virtual void Release(void) override;
	virtual LONGLONG Update(LONGLONG deltaTime, CScreenDIB* dib, DWORD frameCount) override;
	virtual void Draw(CScreenDIB * dib) override;
	virtual void CommunicateNetwork(stPacket_ArgCollectionBox intendBox) override;

public:
	Position	position;

private:
	INT					m_maxHP;
	INT					m_currHP;
	AnimStruct*			m_HpGuageInfo;
	ImageProcessor*	m_imgProcessor;
	CScreenDIB*			m_hpBarBackBuf;
};

