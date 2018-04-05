#pragma once

class GameSystemInfo
{
public:
	~GameSystemInfo();

	/*
	* @breif :  Singleton object�� �ޱ� ���� �Լ��̴�.
	*/
	static GameSystemInfo* GetInstance();

public:
	Constants constants;
	
	HWND	hWnd;
	RECT		GamePlayArea;
	RECT		WindowSize;
	SOCKET	ServerSock;

private:
	GameSystemInfo();
	GameSystemInfo(const GameSystemInfo& obj);
	const GameSystemInfo& operator=(const GameSystemInfo& obj);

private:
	static GameSystemInfo* gameSysInfo;
};

