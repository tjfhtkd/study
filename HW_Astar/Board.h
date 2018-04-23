#pragma once

class CScreenDIB;

class Board
{
public:
	Board(UINT boardWidth, UINT boardHeight, UINT horizontalGridCount, UINT verticalGridCount);
	virtual ~Board();

public:
	UINT GetBoardWidth(void);
	UINT GetBoardHeight(void);
	UINT GetHorizontalGridCount(void);
	UINT GetVerticalGridCount(void);
	MapData** GetMap(void);

	MapData* FillGridAt(INT x, INT y, PIXEL* color);
	void DrawBoard(HWND* hWnd);
	void Clear(void);
	void ClearLink(void);
	void TranslateMousePosToIndex(IN INT* mousePosX, IN INT* mousePosY, OUT INT& out_x, OUT INT& out_y);

private:
	void Draw(HWND* hWnd, CScreenDIB* dibBuf);

private:
	// Unit = pixel
	UINT m_boardWidth;
	UINT m_boardHeight;
	UINT m_horizontalGridCount;
	UINT m_verticalGridCount;
	UINT m_horizontalGridTermSize;
	UINT m_verticalGridTermSize;

	PIXEL m_defaultColor;

	CScreenDIB* dibBuffer;
	MapData** m_grid;
};