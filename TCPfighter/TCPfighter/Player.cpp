#include "stdafx.h"

#include "Player.h"


Player::Player()
{
	int animArrSize	= (int)Status::STATUS_END_SIZE - 1;
	animations		= (Animation**)malloc(sizeof(Animation*) * animArrSize);
	for (int i = 0; i < animArrSize; i++)
	{
		animations[i] = nullptr;
	}
}

Player::~Player()
{
	Release();
}

/////////////////////////////// Public ///////////////////////////////
void Player::SetStatus(Status status)
{
	m_currentStatus = status;
}

Player::Status Player::GetStatus(void)
{
	return m_currentStatus;
}

bool Player::AddAnimations(Animation* anim, Status status)
{
	if (animations[(int)status] == nullptr)
	{
		animations[(int) status] = anim;
		return true;
	}
	return false;
}

Animation* Player::GetAnimation(Status status)
{
	return animations[(int)status];
}

void Player::Move(Status moveStatus)
{
	switch (moveStatus)	
	{
	case Player::Status::MOVE_UP:
		Position.Y -= 2;
		break;
	case Player::Status::MOVE_UPLEFT:
		Position.Y -= 2;
		Position.X -= 3;
		break;
	case Player::Status::MOVE_UPRIGHT:
		Position.Y -= 2;
		Position.X += 3;
		break;
	case Player::Status::MOVE_DOWN:
		Position.Y += 2;
		break;
	case Player::Status::MOVE_DOWN_LEFT:
		Position.Y += 2;
		Position.X -= 3;
		break;
	case Player::Status::MOVE_DOWN_RIGHT:
		Position.Y += 2;
		Position.X += 3;
		break;
	case Player::Status::MOVE_R:
		Position.X += 3;
		break;
	case Player::Status::MOVE_L:
		Position.X -= 3;
		break;
	default:
		break;
	}
}

void Player::Attack(Status attackStatus)
{
}

bool Player::Initialize(void)
{
	m_currentStatus = Status::STAND_L;
	m_imgProcessor = new ImageProcessor();
	Position = { 10, 10 };
	return true;
}

void Player::Release(void)
{
	if (m_imgProcessor != nullptr)
	{
		delete m_imgProcessor;
	}

	int animArrSize = (int)Status::STATUS_END_SIZE - 1;
	for (int i = 0; i < animArrSize; i++)
	{
		delete animations[i];
	}
	free(animations);
}
void Player::KeyProcess(KeyMsg keyMsg)
{
	// 클래스는 하나의 작업군이어야 하고 멤버 함수는 그 작업군에 필요한 작업들의 집합으로 되어야 하는데
	// 지금 KeyManager는 "1개의 작업군 == 1개의 작업" 이므로 Class로 만들 의미가 없는 것 같다.
	// 따라서, 현재 키매니저는 translate key message 라는 기능 하나만을 위해 존재하게 되며,
	// 이 기능은 매우 작으므로 굳이 Class로 만들지 않겠다.

	// 또한 현재 키와 관계가 있는 객체는 플레이어 하나 뿐이므로, 코드 위치는 플레이어에게 둔다.

	// -> 만일 '이 키 메시지를 ~~한테 보내겠다' 라는 기능이 필요하다면 Class로 뽑는 것이 맞다.
	//     그러나 현재 그 일을 GameManager가 대체하고 있다(Observer).
	//     키처리 오버헤드가 발생하면 Observer를 빼야하므로 그 때는 다시 Class로 고려한다.
	
	// 실제 키 값을 내가 정의한 Key값으로 변환
	// 이유? 마스킹 하려고
	int overrappedKeyMsg = (int) KeyMsg::DO_NOTHING;
	if (GetAsyncKeyState(VK_UP))	// 모든 키입력 상황에 대해 처리하겠다. 
	{
		overrappedKeyMsg = overrappedKeyMsg | (int)KeyMsg::UP;
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		overrappedKeyMsg = overrappedKeyMsg | (int)KeyMsg::DOWN;
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		overrappedKeyMsg = overrappedKeyMsg | (int)KeyMsg::LEFT;
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		overrappedKeyMsg = overrappedKeyMsg | (int)KeyMsg::RIGHT;
	}

	if (GetAsyncKeyState('Z'))
	{
		overrappedKeyMsg = overrappedKeyMsg | (int)KeyMsg::ATK_ZAP;
	}

	if (GetAsyncKeyState('X'))
	{
		overrappedKeyMsg = overrappedKeyMsg | (int)KeyMsg::ATK_PAUNCH;
	}

	if (GetAsyncKeyState('C'))
	{
		overrappedKeyMsg = overrappedKeyMsg | (int)KeyMsg::ATK_KICK;
	}

	// 둘이 섞어도 되긴 하겠지만 키보드의 값과 플레이어의 상태는 다르므로 분리했음.
	// 정의한 키에 맞는 Player의 상태 설정
	switch (overrappedKeyMsg)
	{
	case (int) KeyMsg::UP:
		animations[(int)Status::MOVE_UP] = animations[(int)m_currentStatus];
		m_currentStatus = Status::MOVE_UP;
		break;

	case (int)KeyMsg::DOWN:
		animations[(int)Status::MOVE_DOWN] = animations[(int)m_currentStatus];
		m_currentStatus = Status::MOVE_DOWN;
		break;

	case (int)KeyMsg::LEFT:
		m_currentStatus = Status::MOVE_L;
		break;

	case 0x05:	// KeyMsg::UP | KeyMsg::LEFT
		animations[(int)Status::MOVE_UPLEFT] = animations[(int)Status::MOVE_L];
		m_currentStatus = Status::MOVE_UPLEFT;
		break;

	case 0x06:	// KeyMsg::DOWN | KeyMsg::LEFT
		animations[(int)Status::MOVE_DOWN_LEFT] = animations[(int)Status::MOVE_L];
		m_currentStatus = Status::MOVE_DOWN_LEFT;
		break;

	case (int)KeyMsg::RIGHT:
		m_currentStatus = Status::MOVE_R;
		break;

	case 0x09:	// KeyMsg::UP | KeyMsg::RIGHT
		animations[(int)Status::MOVE_UPRIGHT] = animations[(int)Status::MOVE_R];
		m_currentStatus = Status::MOVE_UPRIGHT;
		break;
		
	case 0x0A:	// KeyMsg::DOWN | KeyMsg::RIGHT
		animations[(int)Status::MOVE_DOWN_RIGHT] = animations[(int)Status::MOVE_R];
		m_currentStatus = Status::MOVE_DOWN_RIGHT;
		break;

	case (int)KeyMsg::ATK_ZAP:
		// test
		m_currentStatus = Status::ATTACK_ZAP_L;
		break;

	case (int)KeyMsg::ATK_PAUNCH:
		break;

	case (int)KeyMsg::ATK_KICK:
		break;

	default:
		// 대기 중이라면 이전 상태에 의거해 대기 모션을 정한다.
		switch (m_currentStatus)
		{
		case Status::MOVE_L:
		case Status::MOVE_UPLEFT:
		case Status::MOVE_DOWN_LEFT:
			m_currentStatus = Status::STAND_L;
			break;

		case Status::MOVE_R:
		case Status::MOVE_UPRIGHT:
		case Status::MOVE_DOWN_RIGHT:
			m_currentStatus = Status::STAND_R;
			break;

		default:
			break;
		}
		break;
	}
	this->Move(m_currentStatus);
	this->Attack((Status)((int)m_currentStatus & 0xffff0000));
}

LONGLONG Player::Update(LONGLONG deltaTime, CScreenDIB* dib, DWORD frameCount)
{
	animations[(int)m_currentStatus]->Play(frameCount);
	AnimStruct* animImg = animations[(int)m_currentStatus]->GetCurrentSprite();
	m_imgProcessor->Clipping(animImg, &this->Position, GameSystemInfo::GetInstance()->WindowSize, dib->GetDibBuffer(), 0xffffffff );
	return deltaTime;
}

void Player::Draw(CScreenDIB* dib)
{
}


/////////////////////////////// Private ///////////////////////////////