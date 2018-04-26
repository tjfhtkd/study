#include "stdafx.h"
#include "JPSAlgorithm.h"


JPSAlgorithm::JPSAlgorithm(PIXEL* obstacleColor, MapData** map, INT mapWidth, INT mapHeight)
	:m_obstacleColor(obstacleColor), m_map(map), m_mapWidth(mapWidth), m_mapHeight(mapHeight)
{
	orthologonalLen = 1;
	diagonalLen = sqrt(2.0f);
}


JPSAlgorithm::~JPSAlgorithm()
{
}

MapData* JPSAlgorithm::FindPath(MapData* node)
{
	// 0. ���� ��� openList�� �ֱ�. <- �̰� �ܺο��� ����. �ۿ��� �ݺ��� ���鼭 �����ҰŶ�

	// 1. openList���� �ϳ� ������.   ���� ��带 x�� ����
	// 1.1 x == endNode  ����  -> ã�Ƴ�
	// 1.2 x == nullptr ����  -> ��ã�Ƴ� (���� ��Ȳ��)
	// 2. �� ��Ȳ�� �ƴ϶�� x�� IdentifySuccessors�� ������.
}

bool JPSAlgorithm::IdentifySuccessors(std::queue<MapData*>& inout_successors, MapData* node)
{
	// 1. �̿� ã��
	// 2. for(�̿�)
	// 2.1 ã�Ƴ� �̿��� ���� ��� x�� �������� jump(�̿�, ������) ����
	// 2.2 ã�Ƴ� �̿��� �ִٸ�,   (���� Nn)
	// 2.2.1 Nn �� closeList�� �ִٸ� continue
	// 2.2.2 Nn �� openList�� ���ų� Nn.F�� ���� ��� x �� �������� �ٽ� ����� F���� ������,
	// 2.2.2.1 ���� ���� ���� �����Ѵ�. (** js �����̶� ���� ���� ���ؾ� ��. ���� ����Ʈ�� ���� �ִ��� ���������� ����
	//			���� ����Ʈ�� �ִ��Ŷ�� �װ� ���� �����ؾ� ��. ���ο�Ÿ� �׳� �ٷ� ���� ����Ʈ�� Ǫ��
	return false;
}

MapData* JPSAlgorithm::Jump(JPSNodeDirectionFlag direction, MapData* destMapDataCursor, MapData* cursor, MapData* neighborCursor, PIXEL* obstacleColor)
{
	INT mapWidth = m_mapWidth;
	INT mapHeight = m_mapHeight;

	while (cursor != nullptr)
	{
		if (cursor->idxX < 0 || cursor->idxX > mapWidth || cursor->idxY < 0 || cursor->idxY > mapHeight)
		{
			return nullptr;
		}

		if (cursor->color.data == obstacleColor->data)
		{
			return nullptr;
		}

		if (cursor->idxX == destMapDataCursor->idxX && cursor->idxY == destMapDataCursor->idxY)
		{
			return destMapDataCursor;
		}

		
	}

	return nullptr;
}
