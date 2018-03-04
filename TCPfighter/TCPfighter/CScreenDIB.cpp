#include "stdafx.h"
#include "CScreenDIB.h"


CScreenDIB::CScreenDIB(KsDIB* background)
{
	m_bypBuffer		= nullptr;
	m_background	= background;
	m_width			= background->bmpInfoHeader->bmiHeader.biWidth;
	m_height			= background->bmpInfoHeader->bmiHeader.biHeight;
	m_colorBit		= background->bmpInfoHeader->bmiHeader.biBitCount;
	m_bufferSize		= 0;
	m_pitch			= (m_width * (m_colorBit >> 3) + 3) & ~3;

	CreateDibBuffer(background);
	memcpy_s(m_bypBuffer, m_bufferSize, background->data, m_bufferSize);
}

CScreenDIB::CScreenDIB(int width, int height, int colorBit)
{
	m_bypBuffer		= nullptr;
	m_background	= nullptr;
	m_width			= width;
	m_height			= height;
	m_colorBit		= colorBit;
	m_bufferSize		= 0;
	m_pitch			= (width * (colorBit >> 3) + 3) & ~3;

	CreateDibBuffer(m_width, m_height, m_colorBit);
}

CScreenDIB::~CScreenDIB()
{
	ReleaseDibBuffer();
}

void CScreenDIB::CreateDibBuffer(int width, int height, int colorBit)
{
	int size = height * width * sizeof(PIXEL);

	m_width = width;
	m_height = height;
	m_colorBit = colorBit;
	m_stDibInfo.bmiHeader.biBitCount = colorBit;
	m_stDibInfo.bmiHeader.biClrImportant = 0;
	m_stDibInfo.bmiHeader.biClrUsed = 0;
	m_stDibInfo.bmiHeader.biCompression = BI_RGB;
	m_stDibInfo.bmiHeader.biHeight = -height;
	m_stDibInfo.bmiHeader.biPlanes = 1;
	m_stDibInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
	m_stDibInfo.bmiHeader.biSizeImage = size;
	m_stDibInfo.bmiHeader.biWidth = width;
	m_bufferSize = size;
	m_bypBuffer = (BYTE*)malloc(size);
}

void CScreenDIB::CreateDibBuffer(KsDIB* spriteInfo)
{
	m_colorBit	= spriteInfo->bmpInfoHeader->bmiHeader.biBitCount;
	m_height		= spriteInfo->bmpInfoHeader->bmiHeader.biHeight;
	m_width		= spriteInfo->bmpInfoHeader->bmiHeader.biWidth;
	m_bufferSize = spriteInfo->pitch * m_height;
	
	spriteInfo->bmpInfoHeader->bmiHeader.biHeight = -spriteInfo->bmpInfoHeader->bmiHeader.biHeight;
	
	m_stDibInfo = *spriteInfo->bmpInfoHeader;
	m_bypBuffer = (BYTE*)malloc(m_bufferSize);
}

void CScreenDIB::ReleaseDibBuffer(void)
{
	if (m_bypBuffer != nullptr) {
		free(m_bypBuffer);
	}
}

void CScreenDIB::DrawBuffer(HWND hWnd, int x, int y)
{
	HDC hdc = GetDC(hWnd);
	StretchDIBits(hdc, x, y, m_width, m_height, x, y, m_width, m_height, m_bypBuffer, &m_stDibInfo, DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(hWnd, hdc);
}

void CScreenDIB::ClearBuffer(void)
{
	if (m_background != nullptr)
	{
		memcpy_s(m_bypBuffer, m_bufferSize, m_background->data, m_bufferSize);
	}
	else
	{
		memset(m_bypBuffer, 0x00000000, m_bufferSize);
	}
}

BYTE* CScreenDIB::GetDibBuffer(void)
{
	return m_bypBuffer;
}

int CScreenDIB::GetWidth(void)
{
	return m_width;
}

int CScreenDIB::GetHeight(void)
{
	return m_height;
}

int CScreenDIB::GetPitch(void)
{
	return m_pitch;
}
