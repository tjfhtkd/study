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

// 거지같은 코드가 됨. 오로지 DO NOTHING을 체크하기 위해 overrappedKeyMsg를 받게됨.
// 애초에 서버에서 원하는 패킷 생성에 맞는 키처리가 되는게 더 깔끔할 것 같음.
// 문제는 그렇게 되면 너무 종속적인 코드가 되버림.
// 1. 그냥 그렇게 한다.
// 2. 범용적인 키처리를 한다.
//		-> 키 변경 상태를 감지 할 수 있는 녀석, 아무 키도 안누른 것 역시 상태의 변화임.
void Player::SendActionPacket(int overrappedKeyMsg, Status prevStatus, Status currStatus)
{
	// 이전 상태와 변경된 상태가 달라졌으면 send
	// 무브 -> 공격  (무브 -> 멈춤 -> 공격)
	// 무브 -> 멈춤
	// ㅅi bㅏㄹ  gㅓ지  cㅗ드
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
	intendBox.Direction = (int)currStatus;	// 패킷 번호랑 억지로 껴맞춘 enum class임. 거지같은새퀴
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
	// 공격 우선순위 정하는 용도. 현재는 다 같다고 가정함.
	// (키 입력 자체가 z < x < c 순서라 우선순위가 없는건 아님.)
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
	// 클래스는 하나의 작업군이어야 하고 멤버 함수는 그 작업군에 필요한 작업들의 집합으로 되어야 하는데
	// 지금 KeyManager는 "1개의 작업군 == 1개의 작업" 이므로 Class로 만들 의미가 없는 것 같다.
	// 따라서, 현재 키매니저는 translate key message 라는 기능 하나만을 위해 존재하게 되며,
	// 이 기능은 매우 작으므로 굳이 Class로 만들지 않겠다.

	// 또한 현재 키와 관계가 있는 객체는 플레이어 하나 뿐이므로, 코드 위치는 플레이어에게 둔다.

	// -> 만일 '이 키 메시지를 ~~한테 보내겠다' 라는 기능이 필요하다면 Class로 뽑는 것이 맞다.
	//     그러나 현재 그 일을 GameManager가 대체하고 있다(Observer).
	//     키처리 오버헤드가 발생하면 Observer를 빼야하므로 그 때는 다시 Class로 고려한다.

	// -> 플레이어와 키처리 사이의 관계가 복잡해졌다. class KeyManager로 할 것을 고려.
	//     단, 이 때의 Manager는 전달자/연결자 정도의 역할이 되어야 한다.
	*/

	if (m_bAttack)	// 공격 모션중에는 다른 모션을 취할 수 없다.
	{
		return;
	}

	// 실제 키 값을 내가 정의한 Key값으로 변환
	// 이유? 마스킹 하려고
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

	// 이젠 귀찮음.. 시간도 없음.. 여기는 이제 다 때려넣어지는 구간임.
	Animation* anim = animations[(int)m_currentStatus];
	anim->Play(frameCount);
	AnimStruct* animImg = anim->GetCurrentSprite();
	Position alignedPos = this->position;
	alignedPos.X -= animImg->centerPos.X;
	alignedPos.Y -= animImg->centerPos.Y;
	
	// LONG -> SHORT 해도 문제 없을까?
	// 이미지가 겁나게 초 고해상도였다면??
	// HP 출력 좌표 계산						<- 얘는 왜 중점좌표 0으로 줬지..?
	Position alignedHPPos = position;
	alignedHPPos.X -= Hp->GetCenterPos().X + 30;
	alignedHPPos.Y -= Hp->GetCenterPos().Y - 10;
	Hp->position = alignedHPPos;

	if (m_shadow->On == true)
	{
		// 그림자 출력 좌표 계산
		Position alignedShadowPos = position;
		// 서버에 붙이면서 좌표가 unsigned로 변경됨. 음수일 때 overflow 대책용. y값도 해야하나..?
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

		// 그림자랑 캐릭터 겹치는 부분 블렌딩 할 좌표값 계산하기
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
	// START : 좌표 계산 확인용 코드
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
	// END : 좌표 계산 확인용 코드

	// START : 좌표 계산 확인용 코드
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
	// END : 좌표 계산 확인용 코드
	//*/

	// 캐릭터 이미지 Clipping
	m_imgProcessor->Clipping(animImg, &alignedPos, GameSystemInfo::GetInstance()->WindowSize, dib, animImg->colorKey);
	if (anim->IsEnd())
	{
		m_bAttack = false;
		anim->Reset();
	}

	// Effect 출력
	// 버그 -> 공격한 다음에 m_effectStartFrameNum 값이 0이 되기도 전에 다른 공격을 하면
	// 값이 다시 갱신되어 이전 공격에 대한 이펙트는 나오지 않게 됨.
	// Queue로 관리하면 해결 됨. 나중에 이펙트를 클래스로 만들면서 바꾸기.
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
	if (GetAsyncKeyState(VK_UP) & 0x8000)	// 모든 키입력 상황에 대해 처리하겠다. 
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
			// 가만히 있을 때 공격하는 키가 입력된 경우
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
			// 움직이고 있을 때 공격하는 키가 입력된 경우
			break;
		}
		break;
	}
	return direction;
}

void Player::DeterminePlayAnimation(int overrappedKeyMsg, LookDirection currentDirection)
{
	// 보는 방향에 따른 좌우 이동 및 대기 모션 결정
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

	// 이동 방향에 따른 애니메이션 결정
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

	// 보는 방향에 따른 공격 애니메이션 결정
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
