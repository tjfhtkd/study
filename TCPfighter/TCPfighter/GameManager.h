#pragma once
#include "CGameBase.h"

class ResourceStorage;

class GameManager : public CGameBase
{
public:
	GameManager();
	~GameManager();

private:
	GameManager(const GameManager& ref)
	{
		// GameManager�� �ݵ�� Wdinwos Size�� Game Area Size�� �޾ƾ߸� �ϸ�, ������ ��ü�̴�.
		// ������ ��ü������ �ֻ��� loop������ ����, ȣ��� ���̹Ƿ� singleton�� �ǹ̰� ����.
	};
	
public:
	void Render(void);
	bool isFrameSkip(void);
	DWORD GetExtraSleepTime(void);

	// CGameBase��(��) ���� ��ӵ�
	virtual bool Initialize(void) override;
	virtual void Release(void) override;
	virtual LONGLONG Update(LONGLONG deltaTime, CScreenDIB* dib, DWORD frameCount) override;
	virtual void Draw(CScreenDIB* dib) override;

private:
	bool LoadBMPResources(ResourceStorage* resStore);
	void PrintFPS(LONGLONG deltaTime);
	void CalcFrameSkip(LONGLONG deltaTime);

////////////////////////
// MEMBER VAR  //
////////////////////////
public:
	DWORD	FpsLimit;
	DWORD	FrameLimit;
	DWORD	m_currRenderCnt;
	DWORD	m_currUpdateCnt;
	bool		IsReady;

private:
	CScreenDIB*			m_backBuf;
	ResourceStorage*	m_resStorage;

	// Object Management
	// https://stackoverflow.com/questions/238008/relative-performance-of-stdvector-vs-stdlist-vs-stdslist
	// ���� �����ϴ� �Ϻ��� Ž���ϴ� ���� �����Ƿ� vector ����
	std::vector<CGameBase*> objects;

	// frame skip management.
	DWORD	m_accumulationDelay;
	DWORD	m_deltaTime;
	DWORD	m_extraTime;
	DWORD	m_secCnt;
};