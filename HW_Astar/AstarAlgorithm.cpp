#include "stdafx.h"
#include "AstarAlgorithm.h"


AstarAlgorithm::AstarAlgorithm(PIXEL* obstacleColor, MapData** map, INT mapWidth, INT mapHeight)
	: m_obstacleColor(obstacleColor), m_map(map), m_mapWidth(mapWidth), m_mapHeight(mapHeight)
{
	Weight = 1;
	m_destMapNode = nullptr;
	m_startMapNode = nullptr;
}

AstarAlgorithm::~AstarAlgorithm()
{
}

void AstarAlgorithm::SetStart(MapData* startNode)
{
	if (startNode == nullptr)
	{
		m_startMapNode = nullptr;
	}
	else
	{
		startNode = SetMapData(startNode->idxX, startNode->idxY, 0, nullptr);
		m_startMapNode = startNode;
		m_openList.push_back(startNode);
	}
}

MapData* AstarAlgorithm::GetStart(void)
{
	return m_destMapNode;
}

void AstarAlgorithm::SetDestination(MapData* destination)
{
	m_destMapNode = destination;
}

MapData* AstarAlgorithm::GetDestination(void)
{
	return m_startMapNode;
}

bool AstarAlgorithm::AddOpenList(MapData* data, PIXEL* openListColor)
{
	if (IsAlreadyExistIn(&m_closeList, data) == true)
	{
		return false;
	}

	if (IsAlreadyExistIn(&m_openList, data) == true)
	{
		return false;
	}

	if (data != nullptr)
	{
		data->color = *openListColor;
		m_openList.push_back(data);
		return true;
	}
	return false;
}

void AstarAlgorithm::DeleteOpenList(MapData* data)
{
	m_openList.remove(data);
}

void AstarAlgorithm::DeleteAllOpenList(void)
{
	m_openList.clear();
}

bool AstarAlgorithm::AddCloseList(MapData* data, PIXEL* closeListColor)
{
	if (IsAlreadyExistIn(&m_closeList, data) == true)
	{
		return false;
	}

	if (data != nullptr)
	{
		data->color = *closeListColor;
		m_closeList.push_back(data);
		return true;
	}
	return false;
}

void AstarAlgorithm::DeleteCloseList(MapData * data)
{
	m_closeList.remove(data);
}

void AstarAlgorithm::DeleteAllCloseList(void)
{
	m_closeList.clear();
}

std::list<MapData*>* AstarAlgorithm::GetOpenList(void)
{
	return &m_openList;
}

std::list<MapData*>* AstarAlgorithm::GetCloseList(void)
{
	return &m_closeList;
}

MapData* AstarAlgorithm::FindPath(void)
{
	auto openList= m_openList;
	//while (openList.empty() == false)
	{
		MapData* dataCursor = GetMinMapData(&openList);
		if (dataCursor == nullptr)
		{
			return nullptr;
		}

		if (dataCursor == m_destMapNode)
		{
			return dataCursor;
		}

		PIXEL closeListColor;
		closeListColor.data = 0xff99ff99;
		AddCloseList(dataCursor, (dataCursor == m_startMapNode) ? &dataCursor->color : &closeListColor);
		DeleteOpenList(dataCursor);

		INT baseX = dataCursor->idxX;
		INT baseY = dataCursor->idxY;

		PIXEL openListColor;
		openListColor.data = 0xff0000ff;
		MapData* tmp;

		INT directDirectionWeight = 10;
		INT diagonalWeight = 26;

		tmp = SetMapData(baseX, baseY - 1, directDirectionWeight, dataCursor);			// UP
		if (tmp != nullptr)
		{
			tmp->color = openListColor;
			m_openList.push_back(tmp);
		}

		tmp = SetMapData(baseX, baseY + 1, directDirectionWeight, dataCursor);			// DOWN
		if (tmp != nullptr)
		{
			tmp->color = openListColor;
			m_openList.push_back(tmp);
		}

		tmp = SetMapData(baseX - 1, baseY, directDirectionWeight, dataCursor);			// LEFT
		if (tmp != nullptr)
		{
			tmp->color = openListColor;
			m_openList.push_back(tmp);
		}

		tmp = SetMapData(baseX + 1, baseY, directDirectionWeight, dataCursor);			// RIGHT
		if (tmp != nullptr)
		{
			tmp->color = openListColor;
			m_openList.push_back(tmp);
		}

		tmp = SetMapData(baseX - 1, baseY - 1, diagonalWeight, dataCursor);		// LEFT UP
		if (tmp != nullptr)
		{
			tmp->color = openListColor;
			m_openList.push_back(tmp);
		}

		tmp = SetMapData(baseX + 1, baseY - 1, diagonalWeight, dataCursor);		// RIGHT UP
		if (tmp != nullptr)
		{
			tmp->color = openListColor;
			m_openList.push_back(tmp);
		}

		tmp = SetMapData(baseX - 1, baseY + 1, diagonalWeight, dataCursor);		// LEFT DOWN
		if (tmp != nullptr)
		{
			tmp->color = openListColor;
			m_openList.push_back(tmp);
		}
		
		tmp = SetMapData(baseX + 1, baseY + 1, diagonalWeight, dataCursor);	// DOWN RIGHT
		if (tmp != nullptr)
		{
			tmp->color = openListColor;
			m_openList.push_back(tmp);
		}
	}

	return nullptr;
}

MapData* AstarAlgorithm::GetMinMapData(std::list<MapData*>* searchTarget)
{
	if (searchTarget->empty() == true || searchTarget == nullptr)
	{
		return nullptr;
	}

	auto begin = searchTarget->begin();
	auto end = searchTarget->end();
	INT min = 9999;
	MapData* target = *begin;

	while (begin != end)
	{
		if (min > (*begin)->mapNode.F)
		{
			min = (*begin)->mapNode.F;
			target = *begin;
		}
		begin++;
	}

	return target;
}

bool AstarAlgorithm::IsAlreadyExistIn(std::list<MapData*>* searchTarget, MapData* data)
{
	if (searchTarget->empty() == true || data == nullptr)
	{
		return false;
	}

	auto begin = searchTarget->begin();
	auto end = searchTarget->end();
	while (begin != end)
	{
		if (data->idxX == (*begin)->idxX && 
			data->idxY == (*begin)->idxY)
		{
			return true;
		}
		begin++;
	}
	return false;
}

MapData* AstarAlgorithm::SetMapData(INT idxX, INT idxY, INT diagonalWeight, MapData* parent)
{
	if (idxX < 0 || idxX >= m_mapWidth ||
		idxY < 0 || idxY >= m_mapHeight)
	{
		return nullptr;
	}

	if (m_map[idxX][idxY].color.data == m_obstacleColor->data)
	{
		return nullptr;
	}

	MapData* data = &m_map[idxX][idxY];

	INT G;
	INT H;
	if (IsAlreadyExistIn(&m_closeList, data) == true)
	{
		return nullptr;
	}

	if (IsAlreadyExistIn(&m_openList, data) == true)
	{
		G = parent->mapNode.G + 1;
		if (data->mapNode.G >= G)
		{
			data->mapNode.G = G;
			H = /*parent->mapNode.H | */Weight * (abs(idxX - m_destMapNode->idxX) + abs(idxY - m_destMapNode->idxY)) + diagonalWeight;
			data->mapNode.F = G + H;
			data->parent = parent;
			DeleteOpenList(data);
			return data;
		}
		return nullptr;
	}

	if (parent == nullptr)
	{
		data->parent = nullptr;
		G = data->mapNode.G = 0;
	}
	else
	{
		data->parent = parent;
		G = data->mapNode.G = parent->mapNode.G + 1;
	}
	// 맨허튼 방식 + 대각선과 직선 비율을 조금 다르게
	H = data->mapNode.H = /*data->mapNode.H | */Weight * ((abs(idxX - m_destMapNode->idxX) + abs(idxY - m_destMapNode->idxY)) + diagonalWeight);
	data->mapNode.F = G + H;
	return data;
}