#include "stdafx.h"

#include "GameSystemInfo.h"

GameSystemInfo* GameSystemInfo::gameSysInfo = nullptr;

GameSystemInfo::GameSystemInfo()
{
}

GameSystemInfo::GameSystemInfo(const GameSystemInfo & obj)
{
	// ���� ������ ����
}

const GameSystemInfo& GameSystemInfo::operator=(const GameSystemInfo & obj)
{
	// ���� ���� ������ ����
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