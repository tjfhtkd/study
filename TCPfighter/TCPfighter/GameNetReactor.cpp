#include "stdafx.h"
#include "Player.h"
#include "OtherPlayer.h"
#include "HpGuage.h"
#include "Shadow.h"
#include "ResourceStorage.h"
#include "GameManager.h"
#include "NetMessenger.h"
#include "PacketDefine.h"
#include "GameNetReactor.h"


GameNetReactor::GameNetReactor()
{
}

GameNetReactor::~GameNetReactor()
{
}

Player* GameNetReactor::MakeCharacter(IN bool bOther, IN DWORD ID, IN BYTE Direction, IN WORD X, IN WORD Y, IN BYTE HP)
{
	// Game에 필요한 애니메이션 준비
	ResourceStorage*	resStorage	= m_gameMng->GetResourceStorage();

	Animation translator(1, false);
	AnimStruct* shadow	= translator.MakeAnimationStruct(resStorage->GetResource((Constants::ResourceName::SHADOW)) , NULL, { 32, 4 });
	AnimStruct* hpImg		= translator.MakeAnimationStruct(resStorage->GetResource((Constants::ResourceName::HP)) , NULL, { 0, 0 });

	shadow->colorKey.data	= COLOR_KEY;
	hpImg->colorKey.data		= COLOR_KEY;

	ImageProcessor		imgPrcessor;
	HpGuage*			hpGuage		= new HpGuage(&imgPrcessor, hpImg);
	Shadow*				shadowObj	= new Shadow(&imgPrcessor, shadow);
	Player*				player;
	if (bOther == false)
	{
		player = new Player(hpGuage, &imgPrcessor);
	}
	else
	{
		player = new OtherPlayer(hpGuage, &imgPrcessor);
		player->SetStatus((Direction == dfPACKET_MOVE_DIR_LL) ? Player::Status::STAND_L : Player::Status::STAND_R);
	}

	player->position = {X, Y};
	player->SetShadow(shadowObj);
	player->ID = ID;
	hpGuage->SetMaxHP(100);
	hpGuage->SetCurrHP(HP);
	
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
	return player;
}

void GameNetReactor::SetGameManager(GameManager* gameMng)
{
	if (gameMng != nullptr)
	{
		m_gameMng = gameMng;
	}
}

int GameNetReactor::ReactWhenConnected(NetMessenger* messenger)
{
	// 왜 안오지..?
	return 0;
}

int GameNetReactor::ReactWhenReadable(NetMessenger* messenger)
{
	stPacket_ArgCollectionBox intendBox;
	int type;
	while (messenger->RecvMsg(&type, &intendBox))
	{
		switch (intendBox.PacketType)
		{
		case dfPACKET_SC_CREATE_MY_CHARACTER:
		{
			Player* player = MakeCharacter(false, intendBox.ID, intendBox.Direction, intendBox.X, intendBox.Y, intendBox.HP);
			player->SetMessenger(messenger);
			m_gameMng->PushObject(player);
			break;
		}

		case dfPACKET_SC_CREATE_OTHER_CHARACTER:
		{
			OtherPlayer* otherPlayer = (OtherPlayer*)MakeCharacter(true, intendBox.ID, intendBox.Direction, intendBox.X, intendBox.Y, intendBox.HP);
			otherPlayer->SetMessenger(messenger);
			m_gameMng->PushObject(otherPlayer);
			m_gameMng->CommunicateNetwork(intendBox);
			break;
		}

		case dfPACKET_SC_DELETE_CHARACTER:
		{
			CGameBase* delTarget = m_gameMng->RemoveObject(intendBox.ID);
			if (delTarget != nullptr)
			{
				delTarget->Release();
			}
			break;
		}

		case dfPACKET_SC_MOVE_START:
		case dfPACKET_SC_MOVE_STOP:
		case dfPACKET_SC_ATTACK1:
		case dfPACKET_SC_ATTACK2:
		case dfPACKET_SC_ATTACK3:
		case dfPACKET_SC_DAMAGE:
		{
			m_gameMng->CommunicateNetwork(intendBox);
			break;
		}

		default:
			break;
		}
	}
	return 0;
}

int GameNetReactor::ReactWhenWritable(NetMessenger* messenger)
{
	// 이거는 WouldBlock 에서 풀려서 쓰기 가능한 상태가 된 경우에 오는 것이므로
	// 일반적으로는 아무데서나 Send 할 수 있도록 작성해야 함.
	// 함수만으론 유저가 이를 파악하기 어려워보임.
	messenger->SendMsg(GameSystemInfo::GetInstance()->ServerSock);
	return 0;
}

int GameNetReactor::ReactWhenClose(SOCKET socket)
{
	return closesocket(socket);
}

// Stand Left
void GameNetReactor::MakeAnimation_PlayerStand_L(IN ResourceStorage* resStorage, OUT Player* player)
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
void GameNetReactor::MakeAnimation_PlayerStand_R(IN ResourceStorage* resStorage, OUT Player* player)
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
void GameNetReactor::MakeAnimation_PlayerMove_L(IN ResourceStorage* resStorage, OUT Player* player)
{
	INT startAnimNum = (INT)Constants::ResourceName::Move_L_01;
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
void GameNetReactor::MakeAnimation_PlayerMove_R(IN ResourceStorage* resStorage, OUT Player* player)
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
void GameNetReactor::MakeAnimation_PlayerZap_L(IN ResourceStorage* resStorage, OUT Player* player)
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
void GameNetReactor::MakeAnimation_PlayerZap_R(IN ResourceStorage* resStorage, OUT Player* player)
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
void GameNetReactor::MakeAnimation_PlayerPaunch_R(IN ResourceStorage* resStorage, OUT Player* player)
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
void GameNetReactor::MakeAnimation_PlayerPaunch_L(IN ResourceStorage* resStorage, OUT Player* player)
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
void GameNetReactor::MakeAnimation_PlayerKick_R(IN ResourceStorage* resStorage, OUT Player* player)
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
void GameNetReactor::MakeAnimation_PlayerKick_L(IN ResourceStorage* resStorage, OUT Player* player)
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

void GameNetReactor::MakeAnimation_Effect(IN ResourceStorage * resStorage, OUT Player * player)
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