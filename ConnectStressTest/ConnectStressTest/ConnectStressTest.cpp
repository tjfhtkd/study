// ConnectStressTest.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "StressTest.h"

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
	StressTest connStressTest(1500);
	if (connStressTest.SetServerAddr(L"127.0.0.1", 6000) == false)
	{
		wprintf_s(L"[%d] ", WSAGetLastError());
		return -1;
	}

	bool bRun = true;
	connStressTest.Connect();
	while (bRun)
	{
		connStressTest.Echo();
		//connStressTest.Disconnect(0.05);
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			bRun = false;
		}

		if (connStressTest.CheckOneSec() == true)
		{
			connStressTest.PrintPacketCnt();
			connStressTest.ResetPacketCnt();
		}
	}

    return 0;
}