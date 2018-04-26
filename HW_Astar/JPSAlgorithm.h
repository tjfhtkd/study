#pragma once

enum class JPSNodeDirectionFlag : unsigned int
{
	LEFTUP = 1
	, UP = 2
	, RIGHTUP = 4
	, LEFT = 8
	, RIGHT = 16
	, LEFTDOWN = 32
	, DOWN = 64
	, RIGHTDOWN = 128
};

class JPSAlgorithm
{
public:
	JPSAlgorithm(PIXEL* m_obstacleColor, MapData** map, INT mapWidth, INT mapHeight);
	virtual ~JPSAlgorithm();

public:
	MapData* FindPath(MapData* node);

protected:
	bool IdentifySuccessors(std::queue<MapData*>& inout_successors, MapData* node);
	MapData* Jump(JPSNodeDirectionFlag direction, MapData* destMapDataCursor, MapData* cursor, MapData* neighborCursor, PIXEL* obstacleColor);

private:
	FLOAT orthologonalLen;
	FLOAT diagonalLen;

	PIXEL* m_obstacleColor;
	MapData** m_map;
	INT m_mapWidth;
	INT m_mapHeight;

	std::queue<MapData*> successors;		// == OpenList
};