#include "stdafx.h"

#include "ResourceStorage.h"
#include "BitmapLoader.h"

#include "GameManager.h"


GameManager::GameManager()
{
	Initialize();
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

bool GameManager::IsReady(void)
{
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

ResourceStorage* GameManager::GetResourceStorage(void)
{
	return m_resStorage;
}

void GameManager::PushObject(CGameBase* obj)
{
	if (obj != nullptr)
	{
		objects.push_back(obj);
	}
}

CGameBase* GameManager::PopObject(void)
{
	CGameBase* tmp = objects.back();
	objects.pop_back();
	return tmp;
}

CGameBase* GameManager::RemoveObject(INT targetObjID)
{
	CGameBase* target = nullptr;
	for (auto begin = objects.begin(); begin != objects.end(); begin++)
	{
		if ((*begin)->ID == targetObjID)
		{
			target = *begin;
			objects.erase(begin);
			break;
		}
	}
	return target;
}

std::vector<CGameBase*>::iterator GameManager::GetBegin(void)
{
	return objects.begin();
}

std::vector<CGameBase*>::iterator GameManager::GetEnd(void)
{
	return objects.end();
}

bool GameManager::Initialize(void)
{
	GameSystemInfo::GetInstance()->WindowSize = { 0, 0, 640, 480 };
	GameSystemInfo::GetInstance()->GamePlayArea = { 0, 0, 6400, 6400 };

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
	// ���� �� ��� ���� �� ���� ���װ� �־ ������ ����...
	int width = GameSystemInfo::GetInstance()->WindowSize.right;
	int height = GameSystemInfo::GetInstance()->WindowSize.bottom;
	PIXEL defaultWorldColor;
	defaultWorldColor.data = (DWORD)0xffffffff;
	m_backBuf = new CScreenDIB(width, height, 32, defaultWorldColor);
	//m_backBuf = new CScreenDIB(m_resStorage->GetResource(Constants::ResourceName::MAP));

	cam = new Camera(0, 0, nullptr);
	cam->AdjustCameSpeed(1.0);
	cam->TurnOnFreeCamMode = false;

	map = new Map(this);
	return true;
}

void GameManager::Release(void)
{
	if (m_resStorage != nullptr)
	{
		m_resStorage->ReleaseAllResource();
		delete m_resStorage;
		m_resStorage = nullptr;
	}
	
	if (m_backBuf != nullptr)
	{
		delete m_backBuf;
		m_backBuf = nullptr;
	}

	if (cam != nullptr)
	{
		delete cam;
		cam = nullptr;
	}

	if (map != nullptr)
	{
		delete map;
		map = nullptr;
	}

	objects.clear();
}

void GameManager::KeyProcess(KeyMsg keyMsg)
{
	if (GameSystemInfo::GetInstance()->hWnd != GetFocus())
	{
		return;
	}

	cam->MoveCam();
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->KeyProcess(keyMsg);
	}
}

LONGLONG GameManager::Update(LONGLONG deltaTime, CScreenDIB* dib, DWORD frameCount)
{
	int screenMidX = GameSystemInfo::GetInstance()->WindowSize.right / 2;
	int screenMidY = GameSystemInfo::GetInstance()->WindowSize.bottom / 2;

	map->Update(deltaTime, m_backBuf, frameCount);		// ���� �ֿ켱�������� ���� ó���Ǿ�� ��.
	
	SortBaseY(objects);
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(deltaTime, m_backBuf, m_currUpdateCnt);
		objects[i]->ScreenPos.X = screenMidX + (objects[i]->position.X - cam->CenterPos.X);
		objects[i]->ScreenPos.Y = screenMidY + (objects[i]->position.Y - cam->CenterPos.Y);
	}
	CalcFrameSkip(deltaTime);
	PrintFPS(deltaTime);
	m_currUpdateCnt++;
	return timeGetTime();
}

void GameManager::SortBaseY(std::vector<CGameBase*>& objects)
{
	CGameBase* tmp;
	int iMax = objects.size() - 1;
	for (int i = 0; i < iMax; i++)
	{
		for (int j = i + 1; j < iMax + 1; j++)
		{
			if (objects[i]->position.Y > objects[j]->position.Y)
			{
				tmp = objects[i];
				objects[i] = objects[j];
				objects[j] = tmp;
			}
		}
	}
}

void GameManager::Draw(CScreenDIB* dib)
{
	map->Draw(dib);		// ���� �ֿ켱�������� ���� ó���Ǿ�� ��.
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

void GameManager::CommunicateNetwork(stPacket_ArgCollectionBox intendBox)
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->CommunicateNetwork(intendBox);
	}
}
