// HW_Astar.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Board.h"
#include "AstarAlgorithm.h"
#include "HW_Astar.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

Board* board;
AstarAlgorithm* astarMachine;
PIXEL* selectedGridColor;
PIXEL blackColor;
PIXEL greyColor;
PIXEL redColor;
PIXEL blueColor;
PIXEL greenColor;
PIXEL whiteColor;
bool bClicked;
bool bSetDestinationNode;

// 시작점, 목표점은 전체 맵에서 각각 유일해야 한다.
// 따라서 위치값을 저장해 새 좌표가 설정되면 이전 좌표에 있던 색은 지운다.
COORD startPos;		// 초록색 시작점의 값 저장.
COORD goalPos;		// 붉은색 목표점의 값 저장.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HWASTAR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HWASTAR));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HWASTAR);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		{
			blackColor.data = 0xff000000;
			greyColor.data = 0xffa0a0a0;
			redColor.data = 0xffff0000;
			greenColor.data = 0xff00ff00;
			blueColor.data = 0xff0000ff;
			whiteColor.data = 0xffffffff;
			board = new Board(700, 700, 700/20, 700/20);
			astarMachine = new AstarAlgorithm(&greyColor, board->GetMap(),
				board->GetVerticalGridCount(),
				board->GetHorizontalGridCount());
			selectedGridColor = &greyColor;
			bClicked = false;
			bSetDestinationNode = false;
			InvalidateRect(hWnd, NULL, false);
		}
		break;

	case WM_LBUTTONDOWN:
		{
			COORD pos = { LOWORD(lParam), HIWORD(lParam) };
			board->FillGridAt(pos.X, pos.Y, selectedGridColor);
			if (selectedGridColor == &redColor)
			{
				board->FillGridAt(goalPos.X, goalPos.Y, &whiteColor);
				goalPos = pos;
			}
			else if (selectedGridColor == &greenColor)
			{
				board->FillGridAt(startPos.X, startPos.Y, &whiteColor);
				startPos = pos;
			}
			else
			{
				bClicked = true;
			}
			board->DrawBoard(&hWnd);
		}
		break;

	case WM_MOUSEMOVE:
		{
			if (bClicked == true)
			{
				board->FillGridAt(LOWORD(lParam), HIWORD(lParam), selectedGridColor);
				board->DrawBoard(&hWnd);
			}
		}
		break;

	case WM_LBUTTONUP:
		{
			bClicked = false;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case 0x31:	// number 1
			selectedGridColor = &greyColor;
			break;

		case 0x32:	// number 2
			selectedGridColor = &whiteColor;
			break;

		case 0x33:	// number 3
			selectedGridColor = &redColor;
			break;

		case 0x34:	// number 4
			selectedGridColor = &greenColor;
			break;

		case 0x43:	// Character 'C' or 'c' is clear screen.
		case 0x63:
			board->Clear();
			astarMachine->DeleteAllOpenList();
			astarMachine->DeleteAllCloseList();
			board->DrawBoard(&hWnd);
			break;

		case 0x52:	// Character 'R' or 'r' is run A* algorithm.
		case 0x72:
			{
				//if (astarMachine->GetStart()->color.data == whiteColor.data ||
				//	astarMachine->GetDestination()->color.data == whiteColor.data)
				//{
				//	// 시작점, 목표점이 지워졌어도 수행하면 안된다.
				//	bSetDestinationNode = false;
				//	astarMachine->SetStart(nullptr);
				//	astarMachine->SetDestination(nullptr);
				//	break;
				//}

				MapData** map = board->GetMap();
				INT idxX = goalPos.X;
				INT idxY = goalPos.Y;
				board->TranslateMousePosToIndex(&idxX, &idxY, idxX, idxY);
				astarMachine->SetDestination(&map[idxX][idxY]);
				
				idxX = startPos.X;
				idxY = startPos.Y;
				board->TranslateMousePosToIndex(&idxX, &idxY, idxX, idxY);
				astarMachine->SetStart(&map[idxX][idxY]);

				board->ClearLink();
				MapData* pathRoot = nullptr;
				while (pathRoot == nullptr)
				{
					pathRoot = astarMachine->FindPath();
					//board->DrawBoard(&hWnd);
				}

				while (pathRoot != nullptr)
				{
					pathRoot->color.data = blackColor.data;
					pathRoot = pathRoot->parent;
					board->DrawBoard(&hWnd);
				}
			}
			break;

		default:
			break;
		}
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
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
			board->DrawBoard(&hWnd);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		delete astarMachine;
		delete board;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}