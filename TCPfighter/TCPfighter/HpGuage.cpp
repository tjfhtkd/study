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
		0, 0, right, bottom
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
	COORD alignedPos = Position;
	alignedPos.X -= m_HpGuageInfo->centerPos.X;
	alignedPos.Y -= m_HpGuageInfo->centerPos.Y;
	m_imgProcessor->Clipping(tmp, &alignedPos, GameSystemInfo::GetInstance()->WindowSize, dib, m_HpGuageInfo->colorKey);

	// �� ���¸� ���� �ؾ��ϴ°�...
	free(tmp->sprite);
	free(tmp->sprite->bmpInfoHeader);
	free(tmp);

	return deltaTime;
}

void HpGuage::Draw(CScreenDIB* dib)
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

INT HpGuage::GeCurrentHP(void)
{
	return m_currHP;
}

INT HpGuage::GeMaxHP(void)
{
	return m_maxHP;
}

void HpGuage::SetMaxHP(INT changedMaxHP)
{
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

COORD HpGuage::GetCenterPos(void)
{
	return m_HpGuageInfo->centerPos;
}
