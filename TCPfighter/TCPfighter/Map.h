#pragma once
#include "CGameBase.h"

class GameManager;

class Map : public CGameBase
{
public:
	Map(GameManager* gameMng);
	virtual ~Map();

	// CGameBase을(를) 통해 상속됨
	virtual void CommunicateNetwork(stPacket_ArgCollectionBox intendBox) override;
	virtual bool Initialize(void) override;
	virtual void Release(void) override;
	virtual LONGLONG Update(LONGLONG deltaTime, CScreenDIB* dib, DWORD frameCount) override;
	virtual void Draw(CScreenDIB* dib) override;

private:
	struct Tile
	{
		AnimStruct data;
		Position screenPos;
	};

private:
	Tile tiles[100][100];
	GameManager* m_gameMng;
	ImageProcessor* m_imgProcessor;
};