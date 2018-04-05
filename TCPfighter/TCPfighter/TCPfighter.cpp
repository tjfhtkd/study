// TCPfighter.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include "GameManager.h"
#include "NetProcessor.h"
#include "GameNetReactor.h"
#include "TCPfighter.h"

#define MAX_LOADSTRING		100

// 전역 변수:
static HIMC	g_hOldIMC;
HINSTANCE	hInst;												// 현재 인스턴스입니다.
WCHAR		szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
WCHAR		szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

NetProcessor		netProcessor;
GameManager*	gameMng = new GameManager();
WCHAR g_inputIp[16];

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM						MyRegisterClass(HINSTANCE hInstance);
BOOL						InitInstance(HINSTANCE, int);
LRESULT CALLBACK		WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR	CALLBACK	DialogProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

GameManager*	MakeGameManager(void);
void ReleaseGame(void);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TCPFIGHTER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);


    // 응용 프로그램 초기화를 수행합니다.
	GameSystemInfo::GetInstance()->WindowSize = { 0, 0, 640, 480 };
	GameSystemInfo::GetInstance()->GamePlayArea = { 13, 52, 628, 468 };
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
	// connect가 Dialog 밖에 있는 이유?
	// Dialog에서 OK 할 때 connect 하면 정의된 WM_SOCK(WM_USER + 1)메시지가
	// WinProc으로 오지 않고 DialogProc 으로 간다.
	// WSAAsyncSelect() 등록 할 때 hWnd가 Dialog에서 온 녀석이라 그쪽으로 연결됨.
	// 수정 -> CreateWindow 하면사 나온 hWnd를 등록하게 함.
	/*if (netProcessor.ConnectToServer(GameSystemInfo::GetInstance()->hWnd, g_inputIp, Constants::SERVER_PORT) == false)
	{
		DestroyWindow(GameSystemInfo::GetInstance()->hWnd);
		return FALSE;
	}*/

    // 기본 메시지 루프입니다.
    MSG msg;
	GameManager*	gameMng	= MakeGameManager();
	DWORD			workingTime	= timeGetTime();

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
			gameMng->KeyProcess(KeyMsg::DO_NOTHING);
			workingTime = gameMng->Update(timeGetTime() - workingTime, nullptr, NULL);
			if (gameMng->isFrameSkip() == false)
			{
				Sleep(gameMng->GetExtraSleepTime());
				workingTime = timeGetTime();
				gameMng->Render();
			}
			else {	// Frame Skip 수치 출력용
				wchar_t buf[54];
				wsprintfW(buf, L"FRAME SKIP : %ld fps / %ld flip"
					, gameMng->m_currUpdateCnt
					, gameMng->m_currRenderCnt);
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
	netProcessor.ProcessNetwork(hWnd, message, wParam, lParam);
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
		ReleaseGame();
		ImmAssociateContext(hWnd, g_hOldIMC);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK DialogProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
	{
		// DialogEdit 에 접근하는 방법 1
		/*HWND hEdit = GetDlgItem(hWnd, IDD_DIALOG1);
		SetWindowText(hEdit, L"127.0.0.1");*/
		SetDlgItemText(hWnd, IDC_EDIT_INPUT_IP, L"127.0.0.1");

		GameNetReactor* reactor = new GameNetReactor();
		reactor->SetGameManager(gameMng);
		netProcessor.RegistNetworkReactor(reactor);
		// 여기까지
	}
	return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
		{
			WCHAR strIP[16];
			// DialogEdit 에 접근하는 방법 2
			GetDlgItemText(hWnd, IDC_EDIT_INPUT_IP, strIP, _countof(strIP));
			memcpy_s(g_inputIp, sizeof(strIP), strIP, sizeof(strIP));
			//// 여기까지
			if (netProcessor.ConnectToServer(GameSystemInfo::GetInstance()->hWnd, strIP, Constants::SERVER_PORT) == true)
			{
				GameSystemInfo::GetInstance()->ServerSock = netProcessor.GetServerSock();
				EndDialog(hWnd, 0);
				break;
			}
			SetDlgItemText(hWnd, IDC_EDIT_INPUT_IP, L"ERROR");
		}
		break;

		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

GameManager* MakeGameManager(void)
{
	//gameMng = new GameManager();

	// 게임 초기화 실패는 치명적. 게임을 종료
	if (gameMng->IsReady() == false)
	{
		DestroyWindow(GameSystemInfo::GetInstance()->hWnd);
	}

	// 제대로 게임 준비가 끝나면 프레임 설정으로 마무리
	gameMng->FpsLimit = 50;
	gameMng->FrameLimit = CLOCKS_PER_SEC / gameMng->FpsLimit;

	return gameMng;
}

void ReleaseGame(void)
{
	gameMng->Release();

	delete gameMng;
	gameMng = nullptr;
}