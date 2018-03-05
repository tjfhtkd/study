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
	// Ŭ������ �ϳ��� �۾����̾�� �ϰ� ��� �Լ��� �� �۾����� �ʿ��� �۾����� �������� �Ǿ�� �ϴµ�
	// ���� KeyManager�� "1���� �۾��� == 1���� �۾�" �̹Ƿ� Class�� ���� �ǹ̰� ���� �� ����.
	// ����, ���� Ű�Ŵ����� translate key message ��� ��� �ϳ����� ���� �����ϰ� �Ǹ�,
	// �� ����� �ſ� �����Ƿ� ���� Class�� ������ �ʰڴ�.

	// ���� ���� Ű�� ���谡 �ִ� ��ü�� �÷��̾� �ϳ� ���̹Ƿ�, �ڵ� ��ġ�� �÷��̾�� �д�.

	// -> ���� '�� Ű �޽����� ~~���� �����ڴ�' ��� ����� �ʿ��ϴٸ� Class�� �̴� ���� �´�.
	//     �׷��� ���� �� ���� GameManager�� ��ü�ϰ� �ִ�(Observer).
	//     Űó�� ������尡 �߻��ϸ� Observer�� �����ϹǷ� �� ���� �ٽ� Class�� ����Ѵ�.
	
	// ���� Ű ���� ���� ������ Key������ ��ȯ
	// ����? ����ŷ �Ϸ���
	int overrappedKeyMsg = (int) KeyMsg::DO_NOTHING;
	if (GetAsyncKeyState(VK_UP))	// ��� Ű�Է� ��Ȳ�� ���� ó���ϰڴ�. 
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

	// ���� ��� �Ǳ� �ϰ����� Ű������ ���� �÷��̾��� ���´� �ٸ��Ƿ� �и�����.
	// ������ Ű�� �´� Player�� ���� ����
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
		// ��� ���̶�� ���� ���¿� �ǰ��� ��� ����� ���Ѵ�.
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