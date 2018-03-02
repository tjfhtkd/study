#pragma once

class GameSystemInfo
{
public:
	~GameSystemInfo();

	/*
	* @breif :  Singleton object를 받기 위한 함수이다.
	*/
	static GameSystemInfo* GetInstance();

public:
	Constants constants;
	
	HWND	hWnd;
	RECT		GamePlayArea;
	RECT		WindowSize;

private:
	GameSystemInfo();
	GameSystemInfo(const GameSystemInfo& obj);
	const GameSystemInfo& operator=(const GameSystemInfo& obj);

private:
	static GameSystemInfo* gameSysInfo;
};

