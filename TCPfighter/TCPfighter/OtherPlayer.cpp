#include "stdafx.h"
#include "NetMessenger.h"
#include "PacketDefine.h"
#include "HpGuage.h"
#include "Shadow.h"
#include "OtherPlayer.h"


OtherPlayer::OtherPlayer(HpGuage* hpGuage, ImageProcessor* imageProcessor)
	: Player(hpGuage, imageProcessor)
{
}

OtherPlayer::~OtherPlayer()
{
}

void OtherPlayer::KeyProcess(KeyMsg keyMsg)
{
	int overrappedKeyMsg = MakeOverrapedVirtualKeyMsg(m_currentStatus);
	LookDirection currentDirection = GetLookDirection(overrappedKeyMsg, m_prevDirection);
	DeterminePlayAnimation(overrappedKeyMsg, currentDirection);
	m_prevDirection = currentDirection;
	this->Attack((Status)m_currentStatus);
}

LONGLONG OtherPlayer::Update(LONGLONG deltaTime, CScreenDIB* dib, DWORD frameCount)
{
	Hp->Update(deltaTime, dib, frameCount);

	// ���� ������.. �ð��� ����.. ����� ���� �� �����־����� ������.
	Animation* anim = animations[(int)m_currentStatus];
	anim->Play(frameCount);
	AnimStruct* animImg = anim->GetCurrentSprite();
	Position alignedPos = this->position;
	alignedPos.X -= animImg->centerPos.X;
	alignedPos.Y -= animImg->centerPos.Y;

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
	
	// ĳ���� �̹��� Clipping
	m_imgProcessor->Clipping(animImg, &alignedPos, GameSystemInfo::GetInstance()->WindowSize, dib, animImg->colorKey);
	if (anim->IsEnd())
	{
		anim->Reset();
		if (m_bAttack == true)
		{
			int overrappedKeyMsg = MakeOverrapedVirtualKeyMsg(m_currentStatus);
			LookDirection currentDirection = GetLookDirection(overrappedKeyMsg, m_prevDirection);
			if (currentDirection == LookDirection::LEFT_MOVE || currentDirection == LookDirection::LEFT_STAND)
			{
				m_currentStatus = Player::Status::STAND_L;
			}
			else
			{
				m_currentStatus = Player::Status::STAND_R;
			}
		}
		m_bAttack = false;
	}

	// Effect ���
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

	this->Move(m_currentStatus);
	return deltaTime;
}

void OtherPlayer::CommunicateNetwork(stPacket_ArgCollectionBox intendBox)
{
	switch (intendBox.PacketType)
	{
	case dfPACKET_SC_CREATE_OTHER_CHARACTER:
	{
		Player::Status dirCondition = (Player::Status) intendBox.Direction;
		if (dirCondition == Player::Status::STAND_L || dirCondition == Player::Status::MOVE_L)
		{
			m_prevDirection = LookDirection::LEFT_STAND;
		}
		else
		{
			m_prevDirection = LookDirection::RIGHT_STAND;
		}
		break;
	}

	case dfPACKET_SC_MOVE_START:
		if (intendBox.ID == ID)
		{
			m_currentStatus = (Player::Status) intendBox.Direction;
			position.X = intendBox.X;
			position.Y = intendBox.Y;
			KeyProcess(KeyMsg::DO_NOTHING);
		}
		break;

	case dfPACKET_SC_MOVE_STOP:
		if (intendBox.ID == ID)
		{
			m_currentStatus = (intendBox.Direction == dfPACKET_MOVE_DIR_LL) ?
				Player::Status::STAND_L : Player::Status::STAND_R;
			position.X = intendBox.X;
			position.Y = intendBox.Y;
			KeyProcess(KeyMsg::DO_NOTHING);
		}
		break;

	case dfPACKET_SC_ATTACK1:
		if (intendBox.ID == ID)
		{
			m_currentStatus = (intendBox.Direction == dfPACKET_MOVE_DIR_LL) ?
				Player::Status::ATTACK_ZAP_L : Player::Status::ATTACK_ZAP_R;
			position.X = intendBox.X;
			position.Y = intendBox.Y;
			KeyProcess(KeyMsg::DO_NOTHING);
		}
		break;

	case dfPACKET_SC_ATTACK2:
		if (intendBox.ID == ID)
		{
			m_currentStatus = (intendBox.Direction == dfPACKET_MOVE_DIR_LL) ?
				Player::Status::ATTACK_PAUNCH_L : Player::Status::ATTACK_PAUNCH_R;
			position.X = intendBox.X;
			position.Y = intendBox.Y;
			KeyProcess(KeyMsg::DO_NOTHING);
		}
		break;

	case dfPACKET_SC_ATTACK3:
		if (intendBox.ID == ID)
		{
			m_currentStatus = (intendBox.Direction == dfPACKET_MOVE_DIR_LL) ?
				Player::Status::ATTACK_KICK_L : Player::Status::ATTACK_KICK_R;
			position.X = intendBox.X;
			position.Y = intendBox.Y;
			KeyProcess(KeyMsg::DO_NOTHING);
		}
		break;

	case dfPACKET_SC_DAMAGE:
		if (intendBox.DamageID == ID)
		{
			Hp->SetCurrHP(intendBox.HP);
			KeyProcess(KeyMsg::DO_NOTHING);
			// ����Ʈ ����� �ʿ�
		}
		break;

	default:
		break;
	}
}

int OtherPlayer::MakeOverrapedVirtualKeyMsg(Player::Status netVirtualKey)
{
	int overrappedKeyMsg = (int)KeyMsg::DO_NOTHING;
	switch (netVirtualKey)
	{
	case Player::Status::MOVE_L:
		overrappedKeyMsg = (int)KeyMsg::LEFT;
		break;

	case Player::Status::MOVE_UPLEFT:
		overrappedKeyMsg = (int)KeyMsg::LEFT | (int)KeyMsg::UP;
		break;

	case Player::Status::MOVE_UP:
		overrappedKeyMsg = (int)KeyMsg::UP;
		break;

	case Player::Status::MOVE_UPRIGHT:
		overrappedKeyMsg = (int)KeyMsg::RIGHT | (int)KeyMsg::UP;
		break;

	case Player::Status::MOVE_R:
		overrappedKeyMsg = (int)KeyMsg::RIGHT;
		break;

	case Player::Status::MOVE_DOWN_RIGHT:
		overrappedKeyMsg = (int)KeyMsg::RIGHT | (int)KeyMsg::DOWN;
		break;

	case Player::Status::MOVE_DOWN:
		overrappedKeyMsg = (int)KeyMsg::DOWN;
		break;

	case Player::Status::MOVE_DOWN_LEFT:
		overrappedKeyMsg = (int)KeyMsg::LEFT | (int)KeyMsg::DOWN;
		break;

	case Player::Status::ATTACK_ZAP_R:
	case Player::Status::ATTACK_ZAP_L:
		overrappedKeyMsg = (int)KeyMsg::ATK_ZAP;
		break;

	case Player::Status::ATTACK_PAUNCH_R:
	case Player::Status::ATTACK_PAUNCH_L:
		overrappedKeyMsg = (int)KeyMsg::ATK_PAUNCH;
		break;

	case Player::Status::ATTACK_KICK_R:
	case Player::Status::ATTACK_KICK_L:
		overrappedKeyMsg = (int)KeyMsg::ATK_KICK;
		break;

	default:
		break;
	}

	return overrappedKeyMsg;
}
