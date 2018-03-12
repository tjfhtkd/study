// TCPfighter.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"

#include "GameManager.h"
#include "Player.h"
#include "HpGuage.h"
#include "Shadow.h"
#include "ResourceStorage.h"
#include "TCPfighter.h"

#define MAX_LOADSTRING		100

#define dfDELAY_STAND		5
#define dfDELAY_MOVE		4
#define dfDELAY_ATTACK1	3
#define dfDELAY_ATTACK2	4
#define dfDELAY_ATTACK3	4
#define dfDELAY_EFFECT		3
#define COLOR_KEY			0xffffffff


// 전역 변수:
static HIMC	g_hOldIMC;
HINSTANCE	hInst;												// 현재 인스턴스입니다.
WCHAR		szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
WCHAR		szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM						MyRegisterClass(HINSTANCE hInstance);
BOOL						InitInstance(HINSTANCE, int);
LRESULT CALLBACK		WndProc(HWND, UINT, WPARAM, LPARAM);

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
	GameSystemInfo::GetInstance()->GamePlayArea = { 10, 50, 630, 470 };
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

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

// MOVE
void MakeAnimation_PlayerMove_R(IN ResourceStorage* resStorage, OUT Player* player);
void MakeAnimation_PlayerMove_L(IN ResourceStorage* resStorage, OUT Player* player);
void MakeAnimation_PlayerStand_R(IN ResourceStorage* resStorage, OUT Player* player);
void MakeAnimation_PlayerStand_L(IN ResourceStorage* resStorage, OUT Player* player);

// ATTACK
void MakeAnimation_PlayerZap_R(IN ResourceStorage* resStorage, OUT Player* player);
void MakeAnimation_PlayerZap_L(IN ResourceStorage* resStorage, OUT Player* player);
void MakeAnimation_PlayerPaunch_R(IN ResourceStorage* resStorage, OUT Player* player);
void MakeAnimation_PlayerPaunch_L(IN ResourceStorage* resStorage, OUT Player* player);
void MakeAnimation_PlayerKick_R(IN ResourceStorage* resStorage, OUT Player* player);
void MakeAnimation_PlayerKick_L(IN ResourceStorage* resStorage, OUT Player* player);

// EFFECT
void MakeAnimation_Effect(IN ResourceStorage* resStorage, OUT Player* player);

GameManager*		gameMng;
ImageProcessor*	imgPrcessor;
HpGuage*			hpGuage;
Shadow*				shadowObj;
Player*				player;

GameManager* MakeGameManager(void)
{
	gameMng	= new GameManager();
	imgPrcessor	= new ImageProcessor();

	// Game에 필요한 애니메이션 준비
	ResourceStorage*	resStorage	= gameMng->GetResourceStorage();

	Animation translator(1, false);
	AnimStruct* shadow	= translator.MakeAnimationStruct(resStorage->GetResource((Constants::ResourceName::SHADOW)) , NULL, { 32, 4 });
	AnimStruct* hpImg		= translator.MakeAnimationStruct(resStorage->GetResource((Constants::ResourceName::HP)) , NULL, { 0, 0 });

	shadow->colorKey.data	= COLOR_KEY;
	hpImg->colorKey.data		= COLOR_KEY;

	hpGuage				= new HpGuage(imgPrcessor, hpImg);
	shadowObj			= new Shadow(imgPrcessor, shadow);
	player					= new Player(hpGuage, imgPrcessor);

	SHORT startX = GameSystemInfo::GetInstance()->WindowSize.right / 2;
	SHORT startY = GameSystemInfo::GetInstance()->WindowSize.bottom / 2;
	player->Position = {startX, startY};
	player->SetShadow(shadowObj);
	
	MakeAnimation_PlayerStand_L(resStorage, player);		// 대기모션 - L
	MakeAnimation_PlayerStand_R(resStorage, player);		// 대기모션 - R
	MakeAnimation_PlayerMove_L(resStorage, player);			// 이동모션 - L
	MakeAnimation_PlayerMove_R(resStorage, player);		// 이동모션 - R
	MakeAnimation_PlayerZap_L(resStorage, player);			// Zap 모션 - L
	MakeAnimation_PlayerZap_R(resStorage, player);			// Zap 모션 - R
	MakeAnimation_PlayerPaunch_L(resStorage, player);		// Pauhcn 모션 - L
	MakeAnimation_PlayerPaunch_R(resStorage, player);		// Pauhcn 모션 - R
	MakeAnimation_PlayerKick_L(resStorage, player);			// Kick 모션 - L
	MakeAnimation_PlayerKick_R(resStorage, player);			// Kick 모션 - R
	MakeAnimation_Effect(resStorage, player);					//Effect
	
	gameMng->PushObject(player);
	gameMng->PushObject(hpGuage);

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
	gameMng->Release();	// 사실 파괴자에서도 호출하긴 함. 그냥 외관상 놔둠.
	delete hpGuage;
	hpGuage = nullptr;

	/*delete shadowObj;
	shadowObj = nullptr;*/

	delete player;
	player = nullptr;

	delete gameMng;
	gameMng = nullptr;

	delete imgPrcessor;
	imgPrcessor = nullptr;
}

// Stand Left
void MakeAnimation_PlayerStand_L(IN ResourceStorage* resStorage, OUT Player* player)
{
	INT startAnimNum = (INT)Constants::ResourceName::Stand_L_01;
	INT endAnimNum = (INT)Constants::ResourceName::Stand_L_03;

	Animation* anim = new Animation((endAnimNum - startAnimNum) + 1, true);
	for (int i = startAnimNum; i <= endAnimNum; i++)
	{
		// 우측 보면서 대기하는 동작들 준비
		AnimStruct* stAnim = anim->MakeAnimationStruct(
			resStorage->GetResource((Constants::ResourceName) i)
			, dfDELAY_STAND, { 71, 90 });
		stAnim->colorKey.data = COLOR_KEY;
		// 동작들을 한 묶음의 애니메이션으로 만들기
		anim->AddSprite(stAnim);
	}
	// 만든 한 동작을 player에게 어떤 행동인지 알려주기
	player->AddAnimations(anim, Player::Status::STAND_L);
}

// Stand Right
void MakeAnimation_PlayerStand_R(IN ResourceStorage* resStorage, OUT Player* player)
{
	INT startAnimNum = (INT)Constants::ResourceName::Stand_R_01;
	INT endAnimNum = (INT)Constants::ResourceName::Stand_R_03;

	Animation* anim = new Animation((endAnimNum - startAnimNum) + 1, true);
	for (int i = startAnimNum; i <= endAnimNum; i++)
	{
		// 우측 보면서 대기하는 동작들 준비
		AnimStruct* stAnim = anim->MakeAnimationStruct(
			resStorage->GetResource((Constants::ResourceName) i)
			, dfDELAY_STAND, { 71, 90 });
		stAnim->colorKey.data = COLOR_KEY;
		// 동작들을 한 묶음의 애니메이션으로 만들기
		anim->AddSprite(stAnim);
	}
	// 만든 한 동작을 player에게 어떤 행동인지 알려주기
	player->AddAnimations(anim, Player::Status::STAND_R);
}

// Move Left
void MakeAnimation_PlayerMove_L(IN ResourceStorage* resStorage, OUT Player* player)
{
	INT startAnimNum = (INT) Constants::ResourceName::Move_L_01;
	INT endAnimNum = (INT)Constants::ResourceName::Move_L_12;

	// 좌측으로 움직이는 동작들 준비
	Animation* anim = new Animation((endAnimNum - startAnimNum) + 1, true);
	for (int i = startAnimNum; i <= endAnimNum; i++)
	{
		AnimStruct* stAnim = anim->MakeAnimationStruct(
			resStorage->GetResource((Constants::ResourceName) i)
			, dfDELAY_MOVE, { 71, 90 });
		stAnim->colorKey.data = COLOR_KEY;
		anim->AddSprite(stAnim);
	}
	// 만든 한 동작을 player에게 어떤 행동인지 알려주기
	player->AddAnimations(anim, Player::Status::MOVE_L);
}

// Move Right
void MakeAnimation_PlayerMove_R(IN ResourceStorage* resStorage, OUT Player* player)
{
	INT startAnimNum = (INT)Constants::ResourceName::Move_R_01;
	INT endAnimNum = (INT)Constants::ResourceName::Move_R_12;

	// 좌측으로 움직이는 동작들 준비
	Animation* anim = new Animation((endAnimNum - startAnimNum) + 1, true);
	for (int i = startAnimNum; i <= endAnimNum; i++)
	{
		AnimStruct* stAnim = anim->MakeAnimationStruct(
			resStorage->GetResource((Constants::ResourceName) i)
			, dfDELAY_MOVE, { 71, 90 });
		stAnim->colorKey.data = COLOR_KEY;
		anim->AddSprite(stAnim);
	}
	// 만든 한 동작을 player에게 어떤 행동인지 알려주기
	player->AddAnimations(anim, Player::Status::MOVE_R);
}

// Zap Left
void MakeAnimation_PlayerZap_L(IN ResourceStorage* resStorage, OUT Player* player)
{
	INT startAnimNum = (INT)Constants::ResourceName::Attack1_L_01;
	INT endAnimNum = (INT)Constants::ResourceName::Attack1_L_04;

	// 좌측으로 움직이는 동작들 준비
	Animation* anim = new Animation((endAnimNum - startAnimNum) + 1, false);
	for (int i = startAnimNum; i <= endAnimNum; i++)
	{
		AnimStruct* stAnim = anim->MakeAnimationStruct(
			resStorage->GetResource((Constants::ResourceName) i)
			, dfDELAY_ATTACK1, { 71, 90 });
		stAnim->colorKey.data = COLOR_KEY;
		anim->AddSprite(stAnim);
	}
	// 만든 한 동작을 player에게 어떤 행동인지 알려주기
	player->AddAnimations(anim, Player::Status::ATTACK_ZAP_L);
}

// Zap Right
void MakeAnimation_PlayerZap_R(IN ResourceStorage* resStorage, OUT Player* player)
{
	INT startAnimNum = (INT)Constants::ResourceName::Attack1_R_01;
	INT endAnimNum = (INT)Constants::ResourceName::Attack1_R_04;

	// 좌측으로 움직이는 동작들 준비
	Animation* anim = new Animation((endAnimNum - startAnimNum) + 1, false);
	for (int i = startAnimNum; i <= endAnimNum; i++)
	{
		AnimStruct* stAnim = anim->MakeAnimationStruct(
			resStorage->GetResource((Constants::ResourceName) i)
			, dfDELAY_ATTACK1, { 71, 90 });
		stAnim->colorKey.data = COLOR_KEY;
		anim->AddSprite(stAnim);
	}
	// 만든 한 동작을 player에게 어떤 행동인지 알려주기
	player->AddAnimations(anim, Player::Status::ATTACK_ZAP_R);
}

// Paunch Left
void MakeAnimation_PlayerPaunch_R(IN ResourceStorage* resStorage, OUT Player* player)
{
	INT startAnimNum = (INT)Constants::ResourceName::Attack2_L_01;
	INT endAnimNum = (INT)Constants::ResourceName::Attack2_L_04;

	// 좌측으로 움직이는 동작들 준비
	Animation* anim = new Animation((endAnimNum - startAnimNum) + 1, false);
	for (int i = startAnimNum; i <= endAnimNum; i++)
	{
		AnimStruct* stAnim = anim->MakeAnimationStruct(
			resStorage->GetResource((Constants::ResourceName) i)
			, dfDELAY_ATTACK2, { 71, 90 });
		stAnim->colorKey.data = COLOR_KEY;
		anim->AddSprite(stAnim);
	}
	// 만든 한 동작을 player에게 어떤 행동인지 알려주기
	player->AddAnimations(anim, Player::Status::ATTACK_PAUNCH_L);
}

// Paunch Right
void MakeAnimation_PlayerPaunch_L(IN ResourceStorage* resStorage, OUT Player* player)
{
	INT startAnimNum = (INT)Constants::ResourceName::Attack2_R_01;
	INT endAnimNum = (INT)Constants::ResourceName::Attack2_R_04;

	// 좌측으로 움직이는 동작들 준비
	Animation* anim = new Animation((endAnimNum - startAnimNum) + 1, false);
	for (int i = startAnimNum; i <= endAnimNum; i++)
	{
		AnimStruct* stAnim = anim->MakeAnimationStruct(
			resStorage->GetResource((Constants::ResourceName) i)
			, dfDELAY_ATTACK2, { 71, 90 });
		stAnim->colorKey.data = COLOR_KEY;
		anim->AddSprite(stAnim);
	}
	// 만든 한 동작을 player에게 어떤 행동인지 알려주기
	player->AddAnimations(anim, Player::Status::ATTACK_PAUNCH_R);
}

// Kick Left
void MakeAnimation_PlayerKick_R(IN ResourceStorage* resStorage, OUT Player* player)
{
	INT startAnimNum = (INT)Constants::ResourceName::Attack3_L_01;
	INT endAnimNum = (INT)Constants::ResourceName::Attack3_L_06;

	// 좌측으로 움직이는 동작들 준비
	Animation* anim = new Animation((endAnimNum - startAnimNum) + 1, false);
	for (int i = startAnimNum; i <= endAnimNum; i++)
	{
		AnimStruct* stAnim = anim->MakeAnimationStruct(
			resStorage->GetResource((Constants::ResourceName) i)
			, dfDELAY_ATTACK3, { 71, 90 });
		stAnim->colorKey.data = COLOR_KEY;
		anim->AddSprite(stAnim);
	}
	// 만든 한 동작을 player에게 어떤 행동인지 알려주기
	player->AddAnimations(anim, Player::Status::ATTACK_KICK_L);
}

// Paunch Right
void MakeAnimation_PlayerKick_L(IN ResourceStorage* resStorage, OUT Player* player)
{
	INT startAnimNum = (INT)Constants::ResourceName::Attack3_R_01;
	INT endAnimNum = (INT)Constants::ResourceName::Attack3_R_06;

	// 좌측으로 움직이는 동작들 준비
	Animation* anim = new Animation((endAnimNum - startAnimNum) + 1, false);
	for (int i = startAnimNum; i <= endAnimNum; i++)
	{
		AnimStruct* stAnim = anim->MakeAnimationStruct(
			resStorage->GetResource((Constants::ResourceName) i)
			, dfDELAY_ATTACK3, { 71, 90 });
		stAnim->colorKey.data = COLOR_KEY;
		anim->AddSprite(stAnim);
	}
	// 만든 한 동작을 player에게 어떤 행동인지 알려주기
	player->AddAnimations(anim, Player::Status::ATTACK_KICK_R);
}

void MakeAnimation_Effect(IN ResourceStorage * resStorage, OUT Player * player)
{
	INT startAnimNum = (INT)Constants::ResourceName::xSpark_1;
	INT endAnimNum = (INT)Constants::ResourceName::xSpark_4;

	// 좌측으로 움직이는 동작들 준비
	Animation* anim = new Animation((endAnimNum - startAnimNum) + 1, false);
	for (int i = startAnimNum; i <= endAnimNum; i++)
	{
		AnimStruct* stAnim = anim->MakeAnimationStruct(
			resStorage->GetResource((Constants::ResourceName) i)
			, dfDELAY_EFFECT, { 70, 70 });
		stAnim->colorKey.data = COLOR_KEY;
		anim->AddSprite(stAnim);
	}
	// 만든 한 동작을 player에게 어떤 행동인지 알려주기
	player->SetEffect(anim);
}
