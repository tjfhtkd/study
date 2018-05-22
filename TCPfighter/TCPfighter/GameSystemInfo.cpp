#include "stdafx.h"

#include "GameSystemInfo.h"

GameSystemInfo* GameSystemInfo::gameSysInfo = nullptr;

GameSystemInfo::GameSystemInfo()
{
}

GameSystemInfo::GameSystemInfo(const GameSystemInfo & obj)
{
	// 복사 생성자 막기
}

const GameSystemInfo& GameSystemInfo::operator=(const GameSystemInfo & obj)
{
	// 복사 대입 생성자 막기
	return obj;
}

GameSystemInfo::~GameSystemInfo()
{
}


GameSystemInfo* GameSystemInfo::GetInstance()
{
	if (gameSysInfo == nullptr)
	{
		gameSysInfo = new GameSystemInfo();
	}
	return gameSysInfo;
}