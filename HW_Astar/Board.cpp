#include "stdafx.h"
#include "CScreenDIB.h"

#include "Board.h"


Board::Board(UINT boardWidth, UINT boardHeight, UINT horizontalGridCount, UINT verticalGridCount)
	: m_boardWidth(boardWidth)
	, m_boardHeight(boardHeight)
	, m_horizontalGridCount(horizontalGridCount)
	, m_verticalGridCount(verticalGridCount)
{
	if (boardHeight % 2 != 0)
	{
		boardHeight += boardHeight % 2;
	}

	if (boardWidth % 2 != 0)
	{
		boardWidth += boardWidth % 2;
	}

	PIXEL defaultColor;
	defaultColor.data = 0xffffffff;
	m_defaultColor = defaultColor;
	dibBuffer = new CScreenDIB(boardWidth, boardHeight, 32, defaultColor);

	m_horizontalGridTermSize = boardHeight / horizontalGridCount;
	m_verticalGridTermSize = boardWidth / verticalGridCount;
	
	FLOAT centerPosX = (boardHeight / horizontalGridCount) / 2;
	FLOAT centerPosY = (boardWidth / verticalGridCount) / 2;

	m_grid = (MapData**)malloc(sizeof(MapData) * horizontalGridCount);
	for (int i = 0; i < horizontalGridCount; i++)
	{
		m_grid[i] = (MapData*)malloc(sizeof(MapData) * verticalGridCount);
		for (int j = 0; j < verticalGridCount; j++)
		{
			m_grid[i][j].color = defaultColor;
			m_grid[i][j].idxX = i;
			m_grid[i][j].idxY = j;
			m_grid[i][j].mapNode.X = centerPosX * (i + 1);
			m_grid[i][j].mapNode.Y = centerPosY * (i + 1);
		}
	}
}

Board::~Board()
{
	if (dibBuffer != nullptr)
	{
		delete dibBuffer;
		dibBuffer = nullptr;
	}

	if (m_grid != nullptr)
	{
		for (int i = 0; i < m_verticalGridTermSize; i++)
		{
			if (m_grid[i] != nullptr)
			{
				free(m_grid[i]);
			}
		}
		free(m_grid);
	}
}
/******************************** PUBLIC *********************************/
UINT Board::GetBoardWidth(void)
{
	return m_boardWidth;
}

UINT Board::GetBoardHeight(void)
{
	return m_boardHeight;
}

UINT Board::GetHorizontalGridCount(void)
{
	return m_horizontalGridCount;
}

UINT Board::GetVerticalGridCount(void)
{
	return m_verticalGridCount;
}

MapData** Board::GetMap(void)
{
	return m_grid;
}

MapData* Board::FillGridAt(INT x, INT y, PIXEL* color)
{
	if (x >= m_boardWidth || y >= m_boardHeight)
	{
		return nullptr;
	}

	INT posX;
	INT posY;
	TranslateMousePosToIndex(&x, &y, posX, posY);
	m_grid[posX][posY].color = *color;
	return &(m_grid[posX][posY]);
}

void Board::DrawBoard(HWND* hWnd)
{
	Draw(hWnd, dibBuffer);
	dibBuffer->DrawBuffer(*hWnd);
}

void Board::Clear(void)
{
	UINT horizontalGridCount = m_horizontalGridCount;
	UINT verticalGridCount = m_verticalGridCount;
	PIXEL* defaultColor = &m_defaultColor;
	for (int i = 0; i < horizontalGridCount; i++)
	{
		for (int j = 0; j < verticalGridCount; j++)
		{
			m_grid[i][j].color = *defaultColor;
			m_grid[i][j].mapNode.F = 0;
			m_grid[i][j].mapNode.G = 0;
			m_grid[i][j].mapNode.H = 0;
			m_grid[i][j].parent = nullptr;
		}
	}
}

void Board::ClearLink(void)
{
	UINT horizontalGridCount = m_horizontalGridCount;
	UINT verticalGridCount = m_verticalGridCount;
	PIXEL* defaultColor = &m_defaultColor;
	for (int i = 0; i < horizontalGridCount; i++)
	{
		for (int j = 0; j < verticalGridCount; j++)
		{
			m_grid[i][j].parent = nullptr;
		}
	}
}

void Board::TranslateMousePosToIndex(IN INT* mousePosX, IN INT* mousePosY, OUT INT& out_x, OUT INT& out_y)
{
	int width = m_horizontalGridCount;
	int widthTerm = m_horizontalGridTermSize;
	for (int xi = 0; xi < width; xi++)
	{
		if ((*mousePosX >= xi * widthTerm) && (*mousePosX < (xi* widthTerm) + widthTerm))
		{
			out_x = xi;
			break;
		}
	}

	int height = m_verticalGridCount;
	int heightTerm = m_verticalGridTermSize;
	for (int yi = 0; yi < height; yi++)
	{
		if ((*mousePosY >= yi * heightTerm) && (*mousePosY < (yi* heightTerm) + heightTerm))
		{
			out_y = yi;
			break;
		}
	}
}
/******************************** PRIVATE *********************************/
void Board::Draw(HWND* hWnd, CScreenDIB* dibBuf)
{
	BYTE* buf = dibBuf->GetDibBuffer();
	UINT boardWidth = m_boardWidth;
	UINT boardHeight = m_boardHeight;
	UINT verticalTermSize = m_verticalGridTermSize;
	UINT horizontalTermSize = m_horizontalGridTermSize;
	MapData** localGridCursor = m_grid;

	PIXEL* cursor = (PIXEL*)buf;
	INT outX, outY;
	for (int i = 0; i < boardHeight; i++)
	{
		if (i % verticalTermSize == 0)	// 가로 선 그리기
		{
			for (int j = 0; j < boardWidth; j++)
			{
				cursor[j].data = 0xff000000;
			}
		}
		else
		{
			for (int j = 0; j < boardWidth; j++)
			{
				TranslateMousePosToIndex(&j, &i, outX, outY);
				if (j % horizontalTermSize == 0)		// 세로 선 그리기
				{
					cursor[j].data = 0xff000000;
				}
				else
				{
					cursor[j] = localGridCursor[outX][outY].color;
				}
			}
		}
		cursor = (PIXEL*)(buf + (boardHeight * sizeof(PIXEL) * (i + 1)));
	}
}