// TCPfighter.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include "GameManager.h"
#include "TCPfighter.h"

#define MAX_LOADSTRING		100


// 전역 변수:
static HIMC	g_hOldIMC;
HINSTANCE	hInst;												// 현재 인스턴스입니다.
WCHAR		szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
WCHAR		szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM						MyRegisterClass(HINSTANCE hInstance);
BOOL						InitInstance(HINSTANCE, int);
LRESULT CALLBACK		WndProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TCPFIGHTER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
	GameSystemInfo::GetInstance()->WindowSize = { 0, 0, 640, 480 };
	GameSystemInfo::GetInstance()->GamePlayArea = { 10, 50, 630, 470 };
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    // 기본 메시지 루프입니다.
    MSG msg;
	GameManager gameMng;
	if (gameMng.IsReady == false)
	{
		// 게임 초기화 실패는 치명적. 게임을 종료하도록 한다.
		DestroyWindow(GameSystemInfo::GetInstance()->hWnd);
	}
	gameMng.FpsLimit		= 50;
	gameMng.FrameLimit	= CLOCKS_PER_SEC / gameMng.FpsLimit;
	DWORD workingTime = timeGetTime();
	while (1) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			gameMng.KeyProcess();
			workingTime = gameMng.Update(timeGetTime() - workingTime, nullptr, NULL);
			if (gameMng.isFrameSkip() == false)
			{
				Sleep(gameMng.GetExtraSleepTime());
				workingTime = timeGetTime();
				gameMng.Render();
			}
			else {
				wchar_t buf[54];
				wsprintfW(buf, L"FRAME SKIP : %ld fps / %ld flip", gameMng.m_currUpdateCnt, gameMng.m_currRenderCnt);
				SetWindowText(GameSystemInfo::GetInstance()->hWnd, buf);
			}
		}
	}

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize				= sizeof(WNDCLASSEX);
    wcex.style					= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc		= WndProc;
    wcex.cbClsExtra			= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance			= hInstance;
    wcex.hIcon				= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TCPFIGHTER));
    wcex.hCursor			= LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCEW(IDC_TCPFIGHTER);
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm			= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   RECT windowSize = GameSystemInfo::GetInstance()->WindowSize;
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
	   windowSize.left, windowSize.top, windowSize.right, windowSize.bottom, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   GameSystemInfo::GetInstance()->hWnd = hWnd;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		g_hOldIMC = ImmAssociateContext(hWnd, nullptr);
		SetMenu(hWnd, nullptr);
		break;

	case WM_IME_KEYDOWN:
	case WM_IME_NOTIFY:
		if (g_hOldIMC != nullptr)
		{
			return 0;
		}
		g_hOldIMC = ImmGetContext(hWnd);
		ImmSetConversionStatus(g_hOldIMC, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE);
		ImmReleaseContext(hWnd, g_hOldIMC);
		g_hOldIMC = nullptr;
		break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		ImmAssociateContext(hWnd, g_hOldIMC);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}