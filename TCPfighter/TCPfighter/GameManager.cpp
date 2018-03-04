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

	// 이미지 자료 로딩
	int resourceAmount	= GameSystemInfo::GetInstance()->constants.RESOURCE_AMOUNTS;
	m_resStorage			= new ResourceStorage(resourceAmount);
	if (LoadBMPResources(m_resStorage) == false)
	{
		Release();
		return false;
	}

	// Backbuffer 배경이미지 준비
	m_backBuf = new CScreenDIB(m_resStorage->GetResource(Constants::ResourceName::MAP));

	// Player 생성 - 테스트
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

	// 모든 게임 객체에 대해 초기화가 제대로 되었는지 확인
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
		// 1. 비트맵 이미지를 불러온다. 이 때는 온전한 정규 Bitmap 규격이다.
		BMP* originalBmp		= bmpLoader.LoadBmp(
			GameSystemInfo::GetInstance()->constants.resourcesPath
			, GameSystemInfo::GetInstance()->constants.resourcesFileName[i]);
		
		// 2. 정규 Bitmap 이미지를 이 게임에 맞는 KsDIB 규격으로 변환한다.
		KsDIB* translatedBmp	= imgProcessor.TranslateBmp(originalBmp);
		
		// 3. Bitmap 이미지는 뒤집혀 있으므로 정방향으로 돌린다.
		KsDIB* reversedBmp	= imgProcessor.ReverseBmpData(translatedBmp);

		// 2 + 3. 같이 처리하는 방법, parameter를 바로 bmp 넣어주면 됨.
		//KsDIB* reversedBmp = imgProcessor->ReverseBmpData(originalBmp);

		// 게임 이미지는 하나라도 빠지면 안되므로 false이면 바로 중단하도록 한다.
		if (resStore->AddResource(reversedBmp) == false)
		{
			return false;
		}
	}
	return true;
}

void GameManager::PrintFPS(LONGLONG deltaTime)
{
	// 1. sleep에 사용한 시간을 기준으로 하기
	// 소요한 시간만큼 값을 보정하기 위해 deltaTime을 더해준다.
	//m_secCnt += GetExtraSleepTime() + deltaTime;

	// 2. 50fps 확신하기
	// 어차피 위 시간이 제대로 20ms 마다 Update를 하고 있다면 그냥 20ms 씩 더해줘도 될 것이다.
	// 왜냐하면 애초에 시간이 잘못 흐르게 될 경우 fps 수치가 50 이상이 나올 것이기 때문이다.
	//m_secCnt += FrameLimit;
	
	// 3. DeltaTime이 아닌 진짜 1초 단위로 세기.
	// 이 방법이 보다 정확하다. DeltaTime을 누적시키면 오차값도 같이 누적이 된다.
	// 1초에 한 번만 측정하면 그만큼 오차값의 누적 기회도 감소하게 된다.

	// 결과 : 1이나 2나 소스도 똑같지만 기준을 무엇으로 삼을지만 다름.
	// 1번은 실제로 sleep한 시간을 측정해서 1초를 보는 것.
	// 2번은 진짜 시간이 흐른지 아닌지는 관심 없고 딱 20ms씩만 잰 뒤에
	//   의도한 fps가 맞는지 보는 것 뿐. <- 근데 이건 기준이 뭔가 찝찝함.


	// 1, 2번 근데 시간 측정 자체가 완벽하지 않는데, 저 값이 1000이 되었다고 그게 진짜 1초냐?
	//if (m_secCnt < CLOCKS_PER_SEC)
	// 3번에 맞는 조건문. 이 값이 1000~1011 튀는 정도는 상관 없다.
	DWORD currTimeElapse = (timeGetTime() - m_secCnt);
	if (currTimeElapse < CLOCKS_PER_SEC)
	{
		return;
	}
	m_secCnt = timeGetTime();
	//m_secCnt = currTimeElapse;

	wchar_t buf[34];
	// m_currUpdateCnt + 1 하는 이유 : 시작이 0임.
	// 1로 시작하면 안되는 이유 : Render가 시작하자마자 FrameSkip 당할 수도 있으므로.
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
		// 간혹 로딩이 오래 걸린 경우, 시작하자마자 20ms 이상이 나올 때도 있다.
		// 이런 경우 Sleep을 할 때 -n 값이 되어 무한루프에 빠진다.
		m_deltaTime = deltaTime - FrameLimit;
		m_accumulationDelay += m_deltaTime;
	}
}
