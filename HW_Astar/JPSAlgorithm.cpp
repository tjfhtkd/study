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
	// 0. 시작 노드 openList에 넣기. <- 이건 외부에서 하자. 밖에서 반복문 돌면서 실행할거라서

	// 1. openList에서 하나 꺼내기.   꺼낸 노드를 x라 하자
	// 1.1 x == endNode  종료  -> 찾아냄
	// 1.2 x == nullptr 종료  -> 못찾아냄 (오류 상황임)
	// 2. 위 상황이 아니라면 x를 IdentifySuccessors로 보낸다.
}

bool JPSAlgorithm::IdentifySuccessors(std::queue<MapData*>& inout_successors, MapData* node)
{
	// 1. 이웃 찾기
	// 2. for(이웃)
	// 2.1 찾아낸 이웃과 현재 노드 x를 기준으로 jump(이웃, 현재노드) 수행
	// 2.2 찾아낸 이웃이 있다면,   (이하 Nn)
	// 2.2.1 Nn 이 closeList에 있다면 continue
	// 2.2.2 Nn 이 openList에 없거나 Nn.F가 현재 노드 x 를 기준으로 다시 계산한 F보다 작으면,
	// 2.2.2.1 새로 계산된 값을 세팅한다. (** js 기준이라 로직 조금 변해야 함. 오픈 리스트에 먼저 있는지 없는지부터 보고
	//			오픈 리스트에 있던거라면 그걸 빼서 세팅해야 함. 새로운거면 그냥 바로 오픈 리스트에 푸시
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
