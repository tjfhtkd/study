// HW_ChatServer.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "NetManager.h"
#include <locale>

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
	setlocale(LC_ALL, "Korean");
	NetManager netMng;
	if (netMng.Connect() == true)
	{
		netMng.IsServerRun = true;
		while (netMng.IsServerRun)
		{
			netMng.ProcessNetwork();
		}
	}
	else
	{
		wprintf_s(L"WSAGetLastError = %d\n", WSAGetLastError());
	}

	getchar();
    return 0;
}