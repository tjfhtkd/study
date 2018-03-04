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
		// GameManager는 반드시 Wdinwos Size와 Game Area Size를 받아야만 하며, 유일한 객체이다.
		// 유일한 객체이지만 최상위 loop에서만 생성, 호출될 것이므로 singleton은 의미가 없다.
	};
	
public:
	void Render(void);
	bool isFrameSkip(void);
	DWORD GetExtraSleepTime(void);

	// CGameBase을(를) 통해 상속됨
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
	// 삽입 삭제하는 일보다 탐색하는 일이 많으므로 vector 선택
	std::vector<CGameBase*> objects;

	// frame skip management.
	DWORD	m_accumulationDelay;
	DWORD	m_deltaTime;
	DWORD	m_extraTime;
	DWORD	m_secCnt;
};