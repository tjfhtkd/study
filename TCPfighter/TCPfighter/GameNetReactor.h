#pragma once

#include "INetReactor.h"

#define dfDELAY_STAND		5
#define dfDELAY_MOVE		4
#define dfDELAY_ATTACK1	3
#define dfDELAY_ATTACK2	4
#define dfDELAY_ATTACK3	4
#define dfDELAY_EFFECT		3
#define COLOR_KEY			0xffffffff


class ResourceStorage;
class Player;
class GameManager;
class NetMessenger;

class GameNetReactor : public INetReactor
{
public:
	GameNetReactor();
	virtual ~GameNetReactor();

public:
	Player* MakeCharacter(IN bool bOther, IN DWORD ID, IN BYTE Direction, IN WORD X, IN WORD Y, IN BYTE HP);
	void SetGameManager(GameManager* gameMng);

public:
	// INetReactor을(를) 통해 상속됨
	virtual int ReactWhenConnected(NetMessenger* messenger) override;
	virtual int ReactWhenReadable(NetMessenger* messenger) override;
	virtual int ReactWhenWritable(NetMessenger* messenger) override;
	virtual int ReactWhenClose(SOCKET socket) override;

private:
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

private:
	GameManager* m_gameMng;
};

