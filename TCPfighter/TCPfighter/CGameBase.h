#pragma once

class CScreenDIB;

/*
* @breif : 게임 내의 객체는 반드시 이 클래스를 상속받도록 한다.
*/
class CGameBase
{
public:
	CGameBase();
	~CGameBase();

public:
	// 키처리는 모든 게임 객체가 반드시 수행해야 하는 것은 아니므로 가상함수로 한다.
	virtual KeyMsg* KeyProcess(void);
	
	virtual bool Initialize(void) = 0;
	virtual void Release(void) = 0;

	/*
	* @brief : 모든 게임 객체는 반드시 Update를 통해 어떤 연산이 되어야만 한다. 따라서 순수 가상함수로 강제 구현을 시킨다.
	* @param deltaTime : 이전 update에서 현재 update까지의 시간 간격.
	* @return : Update가 완전히 완료된 시점의 시간값.
	*/
	virtual LONGLONG Update(LONGLONG deltaTime) = 0;

	/*
	* @brief : 배경에 이미지를 그려넣는다.
	* @param dib : 이미지를 그려넣을 배경에 대한 back buffer.
	*/
	virtual void Draw(CScreenDIB* dib) = 0;
};

