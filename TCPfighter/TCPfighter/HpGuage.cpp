#include "stdafx.h"
#include "HpGuage.h"

HpGuage::HpGuage(ImageProcessor* imgProcessor, AnimStruct* hpImg,  INT maxHP)
{
	m_currHP = m_maxHP = maxHP;
	m_imgProcessor = imgProcessor;
	m_HpGuageInfo = hpImg;
	m_hpBarBackBuf = new CScreenDIB(
		hpImg->sprite->bmpInfoHeader->bmiHeader.biWidth
		, hpImg->sprite->bmpInfoHeader->bmiHeader.biHeight
		, hpImg->sprite->bmpInfoHeader->bmiHeader.biBitCount
		, hpImg->colorKey
	);
	//m_HpGuageInfo->sprite->bmpInfoHeader->bmiHeader.biHeight *= (-1); // �̳༮ ����� Height���� ������ �Ǿ��ִ�. DIB�� ������Ŵ.
}

HpGuage::~HpGuage()
{
	Release();
}

//////////////////////////////////// Public ////////////////////////////////////
bool HpGuage::Initialize(void)
{
	if (m_imgProcessor == nullptr || m_HpGuageInfo == nullptr)
	{
		return false;
	}
	return true;
}

void HpGuage::Release(void)
{
	if (m_HpGuageInfo != nullptr)
	{
		free(m_HpGuageInfo);
		m_HpGuageInfo = nullptr;
	}
}

LONGLONG HpGuage::Update(LONGLONG deltaTime, CScreenDIB* dib, DWORD frameCount)
{
	m_hpBarBackBuf->ClearBuffer();
	
	// HP Guage ���̸� �����Ѵ�.
	LONG right = m_HpGuageInfo->sprite->bmpInfoHeader->bmiHeader.biWidth;
	LONG bottom = m_HpGuageInfo->sprite->bmpInfoHeader->bmiHeader.biHeight;
	RECT clippingArea = {
		0, 0, double(right) / double(m_maxHP) * double(m_currHP), bottom
	};

	m_imgProcessor->Clipping(m_HpGuageInfo, nullptr, clippingArea, m_hpBarBackBuf, m_HpGuageInfo->colorKey);

	BYTE* tmpBuf = m_hpBarBackBuf->GetDibBuffer();
	AnimStruct* tmp = (AnimStruct*)malloc(sizeof(AnimStruct));
	tmp->frameDelay = m_HpGuageInfo->frameDelay;
	tmp->centerPos = m_HpGuageInfo->centerPos;
	tmp->colorKey = m_HpGuageInfo->colorKey;

	tmp->sprite = (KsDIB*)malloc(sizeof(KsDIB));
	tmp->sprite->bmpInfoHeader = (BITMAPINFO*)malloc(sizeof(BITMAPINFO));

	//tmp.sprite->bmpInfoHeader
	tmp->sprite->bmpInfoHeader->bmiHeader.biHeight = bottom;
	tmp->sprite->pitch	= (right * (32 >> 3) + 3) & ~3;
	tmp->sprite->data	= (PIXEL*)tmpBuf;

	// HP Guage ��ü�� ȭ���� ����� Ȯ���ϱ� ���� clipping
	m_imgProcessor->Clipping(tmp, &position, GameSystemInfo::GetInstance()->WindowSize, dib, m_HpGuageInfo->colorKey);

	// �� ���¸� ���� �ؾ��ϴ°�...
	free(tmp->sprite);
	free(tmp->sprite->bmpInfoHeader);
	free(tmp);

	return deltaTime;
}

void HpGuage::Draw(CScreenDIB* dib)
{
}

void HpGuage::CommunicateNetwork(stPacket_ArgCollectionBox intendBox)
{
}

//////////////////////////////////// Private ////////////////////////////////////
bool HpGuage::GetDamaged(INT damage)
{
	if (damage > 0 && m_currHP > 0)
	{
		m_currHP -= damage;
		return true;
	}
	return false;
}

INT HpGuage::GetCurrentHP(void)
{
	return m_currHP;
}

INT HpGuage::GetMaxHP(void)
{
	return m_maxHP;
}

void HpGuage::SetMaxHP(INT changedMaxHP)
{
	if (changedMaxHP > 0)
	{
		m_maxHP = changedMaxHP;
	}
}

void HpGuage::SetCurrHP(INT changedCurrentHP)
{
	if (changedCurrentHP >= 0)
	{
		m_currHP = changedCurrentHP;
	}
}

bool HpGuage::IsDead(void)
{
	if (m_currHP <= 0)
	{
		return true;
	}
	return false;
}

void HpGuage::ChangeHpSprite(AnimStruct* hpImg)
{
	if (m_HpGuageInfo != nullptr)
	{
		free(m_HpGuageInfo);
		m_HpGuageInfo = nullptr;
	}
	m_HpGuageInfo = hpImg;
}

Position HpGuage::GetCenterPos(void)
{
	return m_HpGuageInfo->centerPos;
}
