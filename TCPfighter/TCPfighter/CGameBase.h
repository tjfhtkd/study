#pragma once

class CScreenDIB;
class ImageProcessor;

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
	virtual void KeyProcess(KeyMsg keyMsg);
	
	virtual bool Initialize(void) = 0;
	virtual void Release(void) = 0;

	/*
	* @brief : ��� ���� ��ü�� �ݵ�� Update�� ���� � ������ �Ǿ�߸� �Ѵ�. ���� ���� �����Լ��� ���� ������ ��Ų��.
	* @param deltaTime : ���� update���� ���� update������ �ð� ����.
	* @param imgProcessor : �̹��� ó���� ���� ���μ���.
	* @param frameCount : 1�� ���� ���� Update ��ġ ��.
	* @return : Update�� ������ �Ϸ�� ������ �ð���.
	*/
	virtual LONGLONG Update(LONGLONG deltaTime, CScreenDIB* dib, DWORD frameCount) = 0;

	/*
	* @brief : ��濡 �̹����� �׷��ִ´�.
	* @param dib : �̹����� �׷����� ��濡 ���� back buffer.
	*/
	virtual void Draw(CScreenDIB* dib) = 0;
};

