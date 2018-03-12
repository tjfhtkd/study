#pragma once

class CScreenDIB
{
public:
	CScreenDIB(KsDIB* spriteInfo);
	CScreenDIB(int width, int height, int colorBit, PIXEL defaultFillColor);
	virtual ~CScreenDIB();

protected:
	void CreateDibBuffer(KsDIB* background);
	void CreateDibBuffer(int width, int height, int colorBit);
	void ReleaseDibBuffer(void);

public:
	void DrawBuffer(HWND hWnd, int x = 0, int y = 0);
	void ClearBuffer(void);

	BYTE* GetDibBuffer(void);
	int GetWidth(void);
	int GetHeight(void);
	int GetPitch(void);
	int GetBufferSize(void);

protected:
	BITMAPINFO		m_stDibInfo;
	BYTE*				m_bypBuffer;
	KsDIB*			m_background;	// for clear buffer.
	PIXEL				m_defaultFillColor;

	int					m_width;
	int					m_height;
	int					m_pitch;
	int					m_colorBit;
	int					m_bufferSize;
};