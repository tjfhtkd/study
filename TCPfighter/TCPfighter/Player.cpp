#include "stdafx.h"
#include "HpGuage.h"
#include "Shadow.h"
#include "NetMessenger.h"
#include "PacketDefine.h"
#include "Player.h"

Player::Player(HpGuage* hpGuage, ImageProcessor* imageProcessor)
{
	m_imgProcessor = imageProcessor;
	int animArrSize = (int)Status::END_SIZE;
	animations = (Animation**)malloc(sizeof(Animation*) * animArrSize);
	for (int i = 0; i < animArrSize; i++)
	{
		animations[i] = nullptr;
	}
	Hp = hpGuage;
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
		animations[(int)status] = anim;
		return true;
	}
	return false;
}

Animation* Player::GetAnimation(Status status)
{
	return animations[(int)status];
}

void Player::SetShadow(Shadow* shadow)
{
	if (shadow != nullptr)
	{
		m_shadow = shadow;
	}
}

Shadow* Player::GetShadow(void)
{
	return m_shadow;
}

void Player::SetEffect(Animation* effectAnim)
{
	if (effectAnim != nullptr)
	{
		m_effect = effectAnim;
	}
}

void Player::SetMessenger(NetMessenger* messenger)
{
	m_messenger = messenger;
}

// �������� �ڵ尡 ��. ������ DO NOTHING�� üũ�ϱ� ���� overrappedKeyMsg�� �ްԵ�.
// ���ʿ� �������� ���ϴ� ��Ŷ ������ �´� Űó���� �Ǵ°� �� ����� �� ����.
// ������ �׷��� �Ǹ� �ʹ� �������� �ڵ尡 �ǹ���.
// 1. �׳� �׷��� �Ѵ�.
// 2. �������� Űó���� �Ѵ�.
//		-> Ű ���� ���¸� ���� �� �� �ִ� �༮, �ƹ� Ű�� �ȴ��� �� ���� ������ ��ȭ��.
void Player::SendActionPacket(int overrappedKeyMsg, Status prevStatus, Status currStatus)
{
	// ���� ���¿� ����� ���°� �޶������� send
	// ���� -> ����  (���� -> ���� -> ����)
	// ���� -> ����
	// ��i b����  g����  c�ǵ�
	switch (currStatus)
	{
	case Player::Status::MOVE_L:
	case Player::Status::MOVE_UPLEFT:
	case Player::Status::MOVE_UP:
	case Player::Status::MOVE_UPRIGHT:
	case Player::Status::MOVE_R:
	case Player::Status::MOVE_DOWN_RIGHT:
	case Player::Status::MOVE_DOWN:
	case Player::Status::MOVE_DOWN_LEFT:
		if (currStatus != prevStatus)
		{
			AssemblePacket(dfPACKET_CS_MOVE_START, currStatus);
			m_messenger->SendMsg(GameSystemInfo::GetInstance()->ServerSock);
		}
		break;

	case Player::Status::STAND_R:
	case Player::Status::STAND_L:
		if (currStatus != prevStatus)
		{
			int dirStatus = (currStatus == Status::STAND_R) ? dfPACKET_MOVE_DIR_RR : dfPACKET_MOVE_DIR_LL;
			AssemblePacket(dfPACKET_CS_MOVE_STOP, (Status)dirStatus);
			m_messenger->SendMsg(GameSystemInfo::GetInstance()->ServerSock);
		}
		break;

	case Player::Status::ATTACK_ZAP_R:
	case Player::Status::ATTACK_ZAP_L:
	{
		int dirStatus = (currStatus == Status::ATTACK_ZAP_R) ? dfPACKET_MOVE_DIR_RR : dfPACKET_MOVE_DIR_LL;
		switch (prevStatus)
		{
		case Player::Status::MOVE_L:
		case Player::Status::MOVE_UPLEFT:
		case Player::Status::MOVE_UP:
		case Player::Status::MOVE_UPRIGHT:
		case Player::Status::MOVE_R:
		case Player::Status::MOVE_DOWN_RIGHT:
		case Player::Status::MOVE_DOWN:
		case Player::Status::MOVE_DOWN_LEFT:
		{
			AssemblePacket(dfPACKET_CS_MOVE_STOP, (Status)dirStatus);
			//break;
		}
		default:
			break;
		}
		AssemblePacket(dfPACKET_CS_ATTACK1, (Status)dirStatus);
		m_messenger->SendMsg(GameSystemInfo::GetInstance()->ServerSock);
		break;
	}

	case Player::Status::ATTACK_PAUNCH_R:
	case Player::Status::ATTACK_PAUNCH_L:
	{
		int dirStatus = (currStatus == Status::ATTACK_PAUNCH_R) ? dfPACKET_MOVE_DIR_RR : dfPACKET_MOVE_DIR_LL;
		switch (prevStatus)
		{
		case Player::Status::MOVE_L:
		case Player::Status::MOVE_UPLEFT:
		case Player::Status::MOVE_UP:
		case Player::Status::MOVE_UPRIGHT:
		case Player::Status::MOVE_R:
		case Player::Status::MOVE_DOWN_RIGHT:
		case Player::Status::MOVE_DOWN:
		case Player::Status::MOVE_DOWN_LEFT:
		{
			AssemblePacket(dfPACKET_CS_MOVE_STOP, (Status)dirStatus);
			//break;
		}
		default:
			break;
		}
		AssemblePacket(dfPACKET_CS_ATTACK2, (Status)dirStatus);
		m_messenger->SendMsg(GameSystemInfo::GetInstance()->ServerSock);
		break;
	}

	case Player::Status::ATTACK_KICK_R:
	case Player::Status::ATTACK_KICK_L:
	{
		int dirStatus = (currStatus == Status::ATTACK_KICK_R) ? dfPACKET_MOVE_DIR_RR : dfPACKET_MOVE_DIR_LL;
		switch (prevStatus)
		{
		case Player::Status::MOVE_L:
		case Player::Status::MOVE_UPLEFT:
		case Player::Status::MOVE_UP:
		case Player::Status::MOVE_UPRIGHT:
		case Player::Status::MOVE_R:
		case Player::Status::MOVE_DOWN_RIGHT:
		case Player::Status::MOVE_DOWN:
		case Player::Status::MOVE_DOWN_LEFT:
		{
			AssemblePacket(dfPACKET_CS_MOVE_STOP, (Status)dirStatus);
			//break;
		}
		default:
			break;
		}
		AssemblePacket(dfPACKET_CS_ATTACK3, (Status)dirStatus);
		m_messenger->SendMsg(GameSystemInfo::GetInstance()->ServerSock);
	}
		break;

	default:
		break;
	}
}

void Player::AssemblePacket(int packetType, Status currStatus)
{
	stPacket_ArgCollectionBox intendBox;
	intendBox.ID = ID;
	intendBox.PacketType = packetType;
	intendBox.Direction = (int)currStatus;	// ��Ŷ ��ȣ�� ������ ������ enum class��. ������������
	intendBox.X = position.X;
	intendBox.Y = position.Y;
	m_messenger->SaveSendMsg(&intendBox);
}

void Player::Move(Status moveStatus)
{
	switch (moveStatus)
	{
	case Player::Status::MOVE_UP:
		position.Y -= 2;
		break;
	case Player::Status::MOVE_UPLEFT:
		position.Y -= 2;
		position.X -= 3;
		break;
	case Player::Status::MOVE_UPRIGHT:
		position.Y -= 2;
		position.X += 3;
		break;
	case Player::Status::MOVE_DOWN:
		position.Y += 2;
		break;
	case Player::Status::MOVE_DOWN_LEFT:
		position.Y += 2;
		position.X -= 3;
		break;
	case Player::Status::MOVE_DOWN_RIGHT:
		position.Y += 2;
		position.X += 3;
		break;
	case Player::Status::MOVE_R:
		position.X += 3;
		break;
	case Player::Status::MOVE_L:
		position.X -= 3;
		break;
	default:
		break;
	}
	CheckOutOfPlayArea(position);
}

void Player::Attack(Status attackStatus)
{
	// ���� �켱���� ���ϴ� �뵵. ����� �� ���ٰ� ������.
	// (Ű �Է� ��ü�� z < x < c ������ �켱������ ���°� �ƴ�.)
	switch (attackStatus)
	{
	case Player::Status::ATTACK_ZAP_R:
	case Player::Status::ATTACK_ZAP_L:
	case Player::Status::ATTACK_PAUNCH_R:
	case Player::Status::ATTACK_PAUNCH_L:
	case Player::Status::ATTACK_KICK_R:
	case Player::Status::ATTACK_KICK_L:
		m_bAttack = true;
		break;
	default:
		break;
	}
}

bool Player::Initialize(void)
{
	m_effectStartFrameNum = -1;
	m_bAttack = false;
	m_prevDirection = LookDirection::LEFT_STAND;
	m_currentStatus = Status::STAND_L;
	if (m_imgProcessor == nullptr)
	{
		return false;
	}
	Hp->Initialize();
	return true;
}

void Player::Release(void)
{
	if (animations != nullptr)
	{
		free(animations);
		animations = nullptr;
	}

	if (m_shadow != nullptr)
	{
		delete m_shadow;
		m_shadow = nullptr;
	}

	Hp->Release();
}
void Player::KeyProcess(KeyMsg keyMsg)
{
	/*
	// Ŭ������ �ϳ��� �۾����̾�� �ϰ� ��� �Լ��� �� �۾����� �ʿ��� �۾����� �������� �Ǿ�� �ϴµ�
	// ���� KeyManager�� "1���� �۾��� == 1���� �۾�" �̹Ƿ� Class�� ���� �ǹ̰� ���� �� ����.
	// ����, ���� Ű�Ŵ����� translate key message ��� ��� �ϳ����� ���� �����ϰ� �Ǹ�,
	// �� ����� �ſ� �����Ƿ� ���� Class�� ������ �ʰڴ�.

	// ���� ���� Ű�� ���谡 �ִ� ��ü�� �÷��̾� �ϳ� ���̹Ƿ�, �ڵ� ��ġ�� �÷��̾�� �д�.

	// -> ���� '�� Ű �޽����� ~~���� �����ڴ�' ��� ����� �ʿ��ϴٸ� Class�� �̴� ���� �´�.
	//     �׷��� ���� �� ���� GameManager�� ��ü�ϰ� �ִ�(Observer).
	//     Űó�� ������尡 �߻��ϸ� Observer�� �����ϹǷ� �� ���� �ٽ� Class�� ����Ѵ�.

	// -> �÷��̾�� Űó�� ������ ���谡 ����������. class KeyManager�� �� ���� ���.
	//     ��, �� ���� Manager�� ������/������ ������ ������ �Ǿ�� �Ѵ�.
	*/

	if (m_bAttack)	// ���� ����߿��� �ٸ� ����� ���� �� ����.
	{
		return;
	}

	// ���� Ű ���� ���� ������ Key������ ��ȯ
	// ����? ����ŷ �Ϸ���
	int overrappedKeyMsg				= MakeOverrapedKeyMsg();
	LookDirection currentDirection	= GetLookDirection(overrappedKeyMsg, m_prevDirection);
	
	m_prevStatus = m_currentStatus;
	DeterminePlayAnimation(overrappedKeyMsg, currentDirection);
	SendActionPacket(overrappedKeyMsg, m_prevStatus, m_currentStatus);

	this->Move(m_currentStatus);
	this->Attack((Status)m_currentStatus);

	m_prevDirection = currentDirection;
}

LONGLONG Player::Update(LONGLONG deltaTime, CScreenDIB* dib, DWORD frameCount)
{
	Hp->Update(deltaTime, dib, frameCount);

	// ���� ������.. �ð��� ����.. ����� ���� �� �����־����� ������.
	Animation* anim = animations[(int)m_currentStatus];
	anim->Play(frameCount);
	AnimStruct* animImg = anim->GetCurrentSprite();
	Position alignedPos = this->position;
	alignedPos.X -= animImg->centerPos.X;
	alignedPos.Y -= animImg->centerPos.Y;
	
	// LONG -> SHORT �ص� ���� ������?
	// �̹����� �̳��� �� ���ػ󵵿��ٸ�??
	// HP ��� ��ǥ ���						<- ��� �� ������ǥ 0���� ����..?
	Position alignedHPPos = position;
	alignedHPPos.X -= Hp->GetCenterPos().X + 30;
	alignedHPPos.Y -= Hp->GetCenterPos().Y - 10;
	Hp->position = alignedHPPos;

	if (m_shadow->On == true)
	{
		// �׸��� ��� ��ǥ ���
		Position alignedShadowPos = position;
		// ������ ���̸鼭 ��ǥ�� unsigned�� �����. ������ �� overflow ��å��. y���� �ؾ��ϳ�..?
		int signCorrectionX = int(position.X) - int(m_shadow->GetCenterPos().X);
		if (signCorrectionX < 0)
		{
			alignedShadowPos.X = 0;
		}
		else
		{
			alignedShadowPos.X -= m_shadow->GetCenterPos().X;
		}
		alignedShadowPos.Y -= m_shadow->GetCenterPos().Y;
		m_shadow->position = alignedShadowPos;

		// �׸��ڶ� ĳ���� ��ġ�� �κ� ���� �� ��ǥ�� ����ϱ�
		Position blendingStartPos = { alignedPos.X, alignedPos.Y };
		RECT blendingArea = { 0								//left
			, 0														//top
			, animImg->sprite->bmpInfoHeader->bmiHeader.biWidth		//right
			, animImg->sprite->bmpInfoHeader->bmiHeader.biHeight		//bottom
		};

		m_shadow->ProcAlphaBlending(&alignedShadowPos, &blendingArea, dib);
		if (m_shadow->GetBlendedImg() != nullptr)
		{
			m_imgProcessor->Clipping(m_shadow->GetBlendedImg(), &alignedShadowPos, GameSystemInfo::GetInstance()->WindowSize, dib, animImg->colorKey);
		}
	}

	/*
	// START : ��ǥ ��� Ȯ�ο� �ڵ�
	if (alignedPos.X >= 0)
	{
		PIXEL* centerXCheck = (PIXEL*)dib->GetDibBuffer();
		centerXCheck += alignedPos.X;
		for (int i = 0; i < GameSystemInfo::GetInstance()->GamePlayArea.bottom; i++)
		{
			centerXCheck[i * GameSystemInfo::GetInstance()->WindowSize.right].data = 0xff000000;
		}
	}

	if (alignedPos.Y >= 0)
	{
		PIXEL* centerYCheck = (PIXEL*)dib->GetDibBuffer();
		centerYCheck += alignedPos.Y * GameSystemInfo::GetInstance()->WindowSize.right;
		for (int i = 0; i < GameSystemInfo::GetInstance()->WindowSize.right; i++)
		{
			centerYCheck[i].data = 0xff000000;
		}
	}
	// END : ��ǥ ��� Ȯ�ο� �ڵ�

	// START : ��ǥ ��� Ȯ�ο� �ڵ�
	if (Position.X >= 0)
	{
		PIXEL* posXCheck = (PIXEL*)dib->GetDibBuffer();
		posXCheck += Position.X;
		for (int i = 0; i < GameSystemInfo::GetInstance()->GamePlayArea.bottom; i++)
		{
			posXCheck[i * GameSystemInfo::GetInstance()->WindowSize.right].data = 0xffff0000;
		}
	}

	if (Position.Y >= 0)
	{
		PIXEL* posYCheck = (PIXEL*)dib->GetDibBuffer();
		posYCheck += Position.Y * GameSystemInfo::GetInstance()->WindowSize.right;
		for (int i = 0; i < GameSystemInfo::GetInstance()->WindowSize.right; i++)
		{
			posYCheck[i].data = 0xffff0000;
		}
	}
	// END : ��ǥ ��� Ȯ�ο� �ڵ�
	//*/

	// ĳ���� �̹��� Clipping
	m_imgProcessor->Clipping(animImg, &alignedPos, GameSystemInfo::GetInstance()->WindowSize, dib, animImg->colorKey);
	if (anim->IsEnd())
	{
		m_bAttack = false;
		anim->Reset();
	}

	// Effect ���
	// ���� -> ������ ������ m_effectStartFrameNum ���� 0�� �Ǳ⵵ ���� �ٸ� ������ �ϸ�
	// ���� �ٽ� ���ŵǾ� ���� ���ݿ� ���� ����Ʈ�� ������ �ʰ� ��.
	// Queue�� �����ϸ� �ذ� ��. ���߿� ����Ʈ�� Ŭ������ ����鼭 �ٲٱ�.
	if (m_effectStartFrameNum > 0)
	{
		m_effectStartFrameNum--;
	}

	if (m_effectStartFrameNum == 0)
	{
		m_effect->Play(frameCount);
		m_imgProcessor->Clipping(m_effect->GetCurrentSprite(), &m_effectStartPos, GameSystemInfo::GetInstance()->WindowSize, dib, animImg->colorKey);
		if (m_effect->IsEnd())
		{
			m_effectStartFrameNum = -1;
			m_effect->Reset();
		}
	}
	return deltaTime;
}

void Player::Draw(CScreenDIB* dib)
{
}

/////////////////////////////// Private ///////////////////////////////

int Player::MakeOverrapedKeyMsg(void)
{
	int overrappedKeyMsg = (int) KeyMsg::DO_NOTHING;
	if (GetAsyncKeyState(VK_UP) & 0x8000)	// ��� Ű�Է� ��Ȳ�� ���� ó���ϰڴ�. 
	{
		overrappedKeyMsg = overrappedKeyMsg | (int)KeyMsg::UP;
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		overrappedKeyMsg = overrappedKeyMsg | (int)KeyMsg::DOWN;
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		overrappedKeyMsg = overrappedKeyMsg | (int)KeyMsg::LEFT;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		overrappedKeyMsg = overrappedKeyMsg | (int)KeyMsg::RIGHT;
	}

	if (GetAsyncKeyState('Z') & 0x8000)
	{
		overrappedKeyMsg = (int)KeyMsg::ATK_ZAP;
	}

	if (GetAsyncKeyState('X') & 0x8000)
	{
		overrappedKeyMsg = (int)KeyMsg::ATK_PAUNCH;
	}

	if (GetAsyncKeyState('C') & 0x8000)
	{
		overrappedKeyMsg = (int)KeyMsg::ATK_KICK;
	}

	return overrappedKeyMsg;
}

Player::LookDirection Player::GetLookDirection(int overrappedKeyMsg, LookDirection prevDirection)
{
	LookDirection direction;

	switch (overrappedKeyMsg)
	{
	case (int)KeyMsg::DO_NOTHING:
		switch (prevDirection)
		{
		case Player::LookDirection::LEFT_MOVE:
		case Player::LookDirection::LEFT_STAND:
			direction = LookDirection::LEFT_STAND;
			break;

		case Player::LookDirection::RIGHT_MOVE:
		case Player::LookDirection::RIGHT_STAND:
			direction = LookDirection::RIGHT_STAND;
			break;
		default:
			// ������ ���� �� �����ϴ� Ű�� �Էµ� ���
			break;
		}
		break;

	case (int)KeyMsg::LEFT:
	case 0x05:	// KeyMsg::UP | KeyMsg::LEFT
	case 0x06:	// KeyMsg::DOWN | KeyMsg::LEFT
		direction = LookDirection::LEFT_MOVE;
		break;

	case (int)KeyMsg::RIGHT:
	case 0x09:	// KeyMsg::UP | KeyMsg::RIGHT
	case 0x0A:	// KeyMsg::DOWN | KeyMsg::RIGHT
		direction = LookDirection::RIGHT_MOVE;
		break;

	default: // [UP, DOWN]  - [Z, X, C]
		switch (prevDirection)
		{
		case Player::LookDirection::LEFT_MOVE:
		case Player::LookDirection::LEFT_STAND:
			direction = LookDirection::LEFT_MOVE;
			break;

		case Player::LookDirection::RIGHT_MOVE:
		case Player::LookDirection::RIGHT_STAND:
			direction = LookDirection::RIGHT_MOVE;
			break;
		default:
			// �����̰� ���� �� �����ϴ� Ű�� �Էµ� ���
			break;
		}
		break;
	}
	return direction;
}

void Player::DeterminePlayAnimation(int overrappedKeyMsg, LookDirection currentDirection)
{
	// ���� ���⿡ ���� �¿� �̵� �� ��� ��� ����
	switch (currentDirection)
	{
	case Player::LookDirection::LEFT_MOVE:
		m_currentStatus = Status::MOVE_L;
		break;
	case Player::LookDirection::RIGHT_MOVE:
		m_currentStatus = Status::MOVE_R;
		break;
	case Player::LookDirection::LEFT_STAND:
		m_currentStatus = Status::STAND_L;
		break;
	case Player::LookDirection::RIGHT_STAND:
		m_currentStatus = Status::STAND_R;
		break;
	default:
		break;
	}

	// �̵� ���⿡ ���� �ִϸ��̼� ����
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

	case 0x05:	// KeyMsg::UP | KeyMsg::LEFT
		animations[(int)Status::MOVE_UPLEFT] = animations[(int)Status::MOVE_L];
		m_currentStatus = Status::MOVE_UPLEFT;
		break;

	case 0x06:	// KeyMsg::DOWN | KeyMsg::LEFT
		animations[(int)Status::MOVE_DOWN_LEFT] = animations[(int)Status::MOVE_L];
		m_currentStatus = Status::MOVE_DOWN_LEFT;
		break;

	case 0x09:	// KeyMsg::UP | KeyMsg::RIGHT
		animations[(int)Status::MOVE_UPRIGHT] = animations[(int)Status::MOVE_R];
		m_currentStatus = Status::MOVE_UPRIGHT;
		break;
		
	case 0x0A:	// KeyMsg::DOWN | KeyMsg::RIGHT
		animations[(int)Status::MOVE_DOWN_RIGHT] = animations[(int)Status::MOVE_R];
		m_currentStatus = Status::MOVE_DOWN_RIGHT;
		break;

	default:
		break;
	}

	// ���� ���⿡ ���� ���� �ִϸ��̼� ����
	switch (currentDirection)
	{
	case Player::LookDirection::LEFT_MOVE:
	case Player::LookDirection::LEFT_STAND:
		if (overrappedKeyMsg == (int)KeyMsg::ATK_KICK)
		{
			m_currentStatus = Status::ATTACK_KICK_L;
			m_effectStartPos.X = position.X - 135;
			m_effectStartPos.Y = position.Y - 135;
			m_effectStartFrameNum = 10;
		}
		else if (overrappedKeyMsg == (int)KeyMsg::ATK_ZAP)
		{
			m_currentStatus = Status::ATTACK_ZAP_L;
			m_effectStartPos.X = position.X - 130;
			m_effectStartPos.Y = position.Y - 140;
			m_effectStartFrameNum = 1;
		}
		else if(overrappedKeyMsg == (int)KeyMsg::ATK_PAUNCH)
		{
			m_currentStatus = Status::ATTACK_PAUNCH_L;
			m_effectStartPos.X = position.X - 130;
			m_effectStartPos.Y = position.Y - 135;
			m_effectStartFrameNum = 5;
		}
		else {
			// KeyMsg is not Attack key.
		}
		break;

	case Player::LookDirection::RIGHT_MOVE:
	case Player::LookDirection::RIGHT_STAND:
		if (overrappedKeyMsg == (int)KeyMsg::ATK_KICK)
		{
			m_currentStatus = Status::ATTACK_KICK_R;
			m_effectStartPos.X = position.X;
			m_effectStartPos.Y = position.Y - 135;
			m_effectStartFrameNum = 10;
		}
		else if (overrappedKeyMsg == (int)KeyMsg::ATK_ZAP)
		{
			m_currentStatus = Status::ATTACK_ZAP_R;
			m_effectStartPos.X = position.X;
			m_effectStartPos.Y = position.Y - 140;
			m_effectStartFrameNum = 1;
		}
		else if (overrappedKeyMsg == (int)KeyMsg::ATK_PAUNCH)
		{
			m_currentStatus = Status::ATTACK_PAUNCH_R;
			m_effectStartPos.X = position.X;
			m_effectStartPos.Y = position.Y - 135;
			m_effectStartFrameNum = 5;
		}
		else {
			// KeyMsg is not Attack key.
		}
		break;

	default:
		break;
	}
	
}

void Player::CheckOutOfPlayArea(Position& position)
{
	auto area = GameSystemInfo::GetInstance()->GamePlayArea;
	if (position.X <= area.left)
	{
		position.X = area.left;
	}
	else if (position.X >= area.right)
	{
		position.X = area.right;
	}
	else
	{
		// Can moving.
	}

	if (position.Y <= area.top)
	{
		position.Y = area.top;
	}
	else if (position.Y >= area.bottom)
	{
		position.Y = area.bottom;
	}
	else
	{
		// Can moving.
	}
}

void Player::CommunicateNetwork(stPacket_ArgCollectionBox intendBox)
{
	if (intendBox.DamageID == ID)
	{
		if (intendBox.PacketType == dfPACKET_SC_DAMAGE)
		{
			Hp->SetCurrHP(intendBox.HP);
		}
	}
}
