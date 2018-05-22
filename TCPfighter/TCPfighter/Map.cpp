#include "stdafx.h"
#include "GameManager.h"
#include "ResourceStorage.h"
#include "Map.h"


Map::Map(GameManager* gameMng)
{
	m_gameMng = gameMng;
	Initialize();
}

Map::~Map()
{
}

void Map::CommunicateNetwork(stPacket_ArgCollectionBox intendBox)
{
}

bool Map::Initialize(void)
{
	m_imgProcessor = new ImageProcessor();
	auto resStorage = m_gameMng->GetResourceStorage();
	KsDIB* tileImg = resStorage->GetResource((Constants::ResourceName::Tile_01));
	for (int i = 0; i < 100; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			tiles[i][j].data.centerPos.Y = i * 64;
			tiles[i][j].data.centerPos.X = j * 64;
			tiles[i][j].data.colorKey.data = (DWORD)0xffffffff;
			tiles[i][j].data.frameDelay = 0;
			tiles[i][j].data.sprite = tileImg;
			tiles[i][j].screenPos = {0, 0};
		}
	}
	return true;
}

void Map::Release(void)
{
	if (m_imgProcessor != nullptr)
	{
		delete m_imgProcessor;
		m_imgProcessor = nullptr;
	}
}

LONGLONG Map::Update(LONGLONG deltaTime, CScreenDIB* dib, DWORD frameCount)
{
	int screenMidW	= GameSystemInfo::GetInstance()->WindowSize.right / 2;
	int screenMidH	= GameSystemInfo::GetInstance()->WindowSize.bottom / 2;
	Camera* cam				= m_gameMng->cam;
	SHORT camPosX				= cam->CenterPos.X;
	SHORT camPosY				= cam->CenterPos.Y;
	SHORT camAreaLeft			= camPosX - screenMidW - 64;
	SHORT camAreaTop		= camPosY - screenMidH - 64;
	SHORT camAreaRight		= camPosX + screenMidW + 64;
	SHORT camAreaBottom	= camPosY + screenMidH + 64;

	for (int i = 0; i < 100; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			WORD tileCenterX = tiles[i][j].data.centerPos.X;
			WORD tileCenterY = tiles[i][j].data.centerPos.Y;
			if (camAreaLeft <= tileCenterX && camAreaRight >= tileCenterX
				&& camAreaTop <= tileCenterY && camAreaBottom >= tileCenterY)
			{
				tiles[i][j].screenPos.X = screenMidW + tileCenterX - camPosX;
				tiles[i][j].screenPos.Y = screenMidH + tileCenterY - camPosY;
				m_imgProcessor->Clipping(&tiles[i][j].data, &tiles[i][j].screenPos, GameSystemInfo::GetInstance()->WindowSize, dib, tiles[i][j].data.colorKey);
			}
		}
	}
	return deltaTime;
}

void Map::Draw(CScreenDIB* dib)
{
}