#pragma once

class CScreenDIB;

/*
* @breif : ���� ���� ��ü�� �ݵ�� �� Ŭ������ ��ӹ޵��� �Ѵ�.
*/
class CGameBase
{
public:
	CGameBase();
	~CGameBase();

public:
	// Űó���� ��� ���� ��ü�� �ݵ�� �����ؾ� �ϴ� ���� �ƴϹǷ� �����Լ��� �Ѵ�.
	virtual KeyMsg* KeyProcess(void);
	
	virtual bool Initialize(void) = 0;
	virtual void Release(void) = 0;

	/*
	* @brief : ��� ���� ��ü�� �ݵ�� Update�� ���� � ������ �Ǿ�߸� �Ѵ�. ���� ���� �����Լ��� ���� ������ ��Ų��.
	* @param deltaTime : ���� update���� ���� update������ �ð� ����.
	* @return : Update�� ������ �Ϸ�� ������ �ð���.
	*/
	virtual LONGLONG Update(LONGLONG deltaTime) = 0;

	/*
	* @brief : ��濡 �̹����� �׷��ִ´�.
	* @param dib : �̹����� �׷����� ��濡 ���� back buffer.
	*/
	virtual void Draw(CScreenDIB* dib) = 0;
};

