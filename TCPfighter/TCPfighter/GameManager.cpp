#include "stdafx.h"

#include "ResourceStorage.h"
#include "BitmapLoader.h"

// Test - clipping & Animation
#include "Player.h"

#include "GameManager.h"


GameManager::GameManager()
{
	IsReady = Initialize();
}


GameManager::~GameManager()
{
	Release();
}

/////////////////////////////// Public ///////////////////////////////
void GameManager::Render(void)
{
	m_currRenderCnt++;
	this->Draw(m_backBuf);
}

bool GameManager::isFrameSkip(void)
{
	if (m_accumulationDelay > FrameLimit)
	{
		m_accumulationDelay -= FrameLimit;
		return true;
	}
	return false;
}

DWORD GameManager::GetExtraSleepTime(void)
{
	return FrameLimit - m_deltaTime;
}

bool GameManager::Initialize(void)
{
	timeBeginPeriod(1);
	m_currRenderCnt = m_currUpdateCnt = m_secCnt = m_deltaTime = 0;
	m_secCnt = timeGetTime();

	// �̹��� �ڷ� �ε�
	int resourceAmount	= GameSystemInfo::GetInstance()->constants.RESOURCE_AMOUNTS;
	m_resStorage			= new ResourceStorage(resourceAmount);
	if (LoadBMPResources(m_resStorage) == false)
	{
		Release();
		return false;
	}

	// Backbuffer ����̹��� �غ�
	m_backBuf = new CScreenDIB(m_resStorage->GetResource(Constants::ResourceName::MAP));

	// Player ���� - �׽�Ʈ
	Player* player = new Player();
	player->m_animStandLeft = new Animation(3);
	
	AnimStruct* stand1 = player->m_animStandLeft->MakeAnimationStruct(
		m_resStorage->GetResource(Constants::ResourceName::Stand_L_01)
	, 2, { 20, 20 } );
	AnimStruct* stand2 = player->m_animStandLeft->MakeAnimationStruct(
		m_resStorage->GetResource(Constants::ResourceName::Stand_L_02)
		, 2, { 20, 20 });
	AnimStruct* stand3 = player->m_animStandLeft->MakeAnimationStruct(
		m_resStorage->GetResource(Constants::ResourceName::Stand_L_03)
		, 2, { 20, 20 });

	player->m_animStandLeft->AddSprite(stand1);
	player->m_animStandLeft->AddSprite(stand2);
	player->m_animStandLeft->AddSprite(stand3);
	objects.push_back(player);

	// ��� ���� ��ü�� ���� �ʱ�ȭ�� ����� �Ǿ����� Ȯ��
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i]->Initialize() == false)
		{
			return false;
		}
	}

	return true;
}

void GameManager::Release(void)
{
	m_resStorage->ReleaseAllResource();
	if (m_resStorage != nullptr)
	{
		delete m_resStorage;
	}
	
	if (m_backBuf != nullptr)
	{
		delete m_backBuf;
	}

	objects.clear();
}

LONGLONG GameManager::Update(LONGLONG deltaTime, CScreenDIB* dib, DWORD frameCount)
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(deltaTime, m_backBuf, m_currUpdateCnt);
	}
	CalcFrameSkip(deltaTime);
	PrintFPS(deltaTime);
	m_currUpdateCnt++;
	return timeGetTime();
}

void GameManager::Draw(CScreenDIB* dib)
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Draw(dib);
	}
	dib->DrawBuffer(GameSystemInfo::GetInstance()->hWnd, 0, 0);
	dib->ClearBuffer();
}

/////////////////////////////// Private ///////////////////////////////
bool GameManager::LoadBMPResources(ResourceStorage* resStore)
{
	ImageProcessor imgProcessor;
	BitmapLoader bmpLoader;
	int storeSize = resStore->GetCapacity();
	for (int i = 0; i < storeSize; i++)
	{
		// 1. ��Ʈ�� �̹����� �ҷ��´�. �� ���� ������ ���� Bitmap �԰��̴�.
		BMP* originalBmp		= bmpLoader.LoadBmp(
			GameSystemInfo::GetInstance()->constants.resourcesPath
			, GameSystemInfo::GetInstance()->constants.resourcesFileName[i]);
		
		// 2. ���� Bitmap �̹����� �� ���ӿ� �´� KsDIB �԰����� ��ȯ�Ѵ�.
		KsDIB* translatedBmp	= imgProcessor.TranslateBmp(originalBmp);
		
		// 3. Bitmap �̹����� ������ �����Ƿ� ���������� ������.
		KsDIB* reversedBmp	= imgProcessor.ReverseBmpData(translatedBmp);

		// 2 + 3. ���� ó���ϴ� ���, parameter�� �ٷ� bmp �־��ָ� ��.
		//KsDIB* reversedBmp = imgProcessor->ReverseBmpData(originalBmp);

		// ���� �̹����� �ϳ��� ������ �ȵǹǷ� false�̸� �ٷ� �ߴ��ϵ��� �Ѵ�.
		if (resStore->AddResource(reversedBmp) == false)
		{
			return false;
		}
	}
	return true;
}

void GameManager::PrintFPS(LONGLONG deltaTime)
{
	// 1. sleep�� ����� �ð��� �������� �ϱ�
	// �ҿ��� �ð���ŭ ���� �����ϱ� ���� deltaTime�� �����ش�.
	//m_secCnt += GetExtraSleepTime() + deltaTime;

	// 2. 50fps Ȯ���ϱ�
	// ������ �� �ð��� ����� 20ms ���� Update�� �ϰ� �ִٸ� �׳� 20ms �� �����൵ �� ���̴�.
	// �ֳ��ϸ� ���ʿ� �ð��� �߸� �帣�� �� ��� fps ��ġ�� 50 �̻��� ���� ���̱� �����̴�.
	//m_secCnt += FrameLimit;
	
	// 3. DeltaTime�� �ƴ� ��¥ 1�� ������ ����.
	// �� ����� ���� ��Ȯ�ϴ�. DeltaTime�� ������Ű�� �������� ���� ������ �ȴ�.
	// 1�ʿ� �� ���� �����ϸ� �׸�ŭ �������� ���� ��ȸ�� �����ϰ� �ȴ�.

	// ��� : 1�̳� 2�� �ҽ��� �Ȱ����� ������ �������� �������� �ٸ�.
	// 1���� ������ sleep�� �ð��� �����ؼ� 1�ʸ� ���� ��.
	// 2���� ��¥ �ð��� �帥�� �ƴ����� ���� ���� �� 20ms���� �� �ڿ�
	//   �ǵ��� fps�� �´��� ���� �� ��. <- �ٵ� �̰� ������ ���� ������.


	// 1, 2�� �ٵ� �ð� ���� ��ü�� �Ϻ����� �ʴµ�, �� ���� 1000�� �Ǿ��ٰ� �װ� ��¥ 1�ʳ�?
	//if (m_secCnt < CLOCKS_PER_SEC)
	// 3���� �´� ���ǹ�. �� ���� 1000~1011 Ƣ�� ������ ��� ����.
	DWORD currTimeElapse = (timeGetTime() - m_secCnt);
	if (currTimeElapse < CLOCKS_PER_SEC)
	{
		return;
	}
	m_secCnt = timeGetTime();
	//m_secCnt = currTimeElapse;

	wchar_t buf[34];
	// m_currUpdateCnt + 1 �ϴ� ���� : ������ 0��.
	// 1�� �����ϸ� �ȵǴ� ���� : Render�� �������ڸ��� FrameSkip ���� ���� �����Ƿ�.
	wsprintfW(buf, L"%ld fps / %ld flip / time %ld", m_currUpdateCnt + 1, m_currRenderCnt + 1, currTimeElapse);
	SetWindowText(GameSystemInfo::GetInstance()->hWnd, buf);
	m_currUpdateCnt = 0;
	m_currRenderCnt = 0;
	//m_secCnt %= CLOCKS_PER_SEC;
}

void GameManager::CalcFrameSkip(LONGLONG deltaTime)
{
	m_deltaTime = deltaTime;
	if (deltaTime > FrameLimit)
	{
		// ��Ȥ �ε��� ���� �ɸ� ���, �������ڸ��� 20ms �̻��� ���� ���� �ִ�.
		// �̷� ��� Sleep�� �� �� -n ���� �Ǿ� ���ѷ����� ������.
		m_deltaTime = deltaTime - FrameLimit;
		m_accumulationDelay += m_deltaTime;
	}
}
