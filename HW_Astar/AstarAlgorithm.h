#pragma once
class AstarAlgorithm
{
public:
	AstarAlgorithm(PIXEL* m_obstacleColor, MapData** map, INT mapWidth, INT mapHeight);
	virtual ~AstarAlgorithm();

public:
	void SetStart(MapData* startNode);
	MapData* GetStart(void);
	
	void SetDestination(MapData* destination);
	MapData* GetDestination(void);

	// 해당 데이터가 이미 CloseList에 있다면 값을 넣지 않으며 false를 반환한다.
	bool AddOpenList(MapData* data, PIXEL* openListColor);
	void DeleteOpenList(MapData* data);
	void DeleteAllOpenList(void);

	bool AddCloseList(MapData* data, PIXEL* closeListColor);
	void DeleteCloseList(MapData* data);
	void DeleteAllCloseList(void);

	std::list<MapData*>* GetOpenList(void);
	std::list<MapData*>* GetCloseList(void);

	MapData* FindPath(void);

public:
	INT Weight;

private:
	MapData* GetMinMapData(std::list<MapData*>* searchTarget);
	bool IsAlreadyExistIn(std::list<MapData*>* searchTarget, MapData* data);
	MapData* SetMapData(INT idxX, INT idxY, INT diagonalWeight, MapData* parent);

private:
	INT m_mapWidth;
	INT m_mapHeight;
	MapData* m_destMapNode;
	MapData* m_startMapNode;
	PIXEL* m_obstacleColor;
	MapData** m_map;
	std::list<MapData*> m_openList;
	std::list<MapData*> m_closeList;
};

