#pragma once
#include "Player.h"

class OtherPlayer : public Player
{
public:
	OtherPlayer(HpGuage* hpGuage, ImageProcessor* imageProcessor);
	virtual ~OtherPlayer();

public:
	void KeyProcess(KeyMsg keyMsg) override;
	LONGLONG Update(LONGLONG deltaTime, CScreenDIB * dib, DWORD frameCount) override;
	virtual void CommunicateNetwork(stPacket_ArgCollectionBox intendBox) override;

private:
	int MakeOverrapedVirtualKeyMsg(Player::Status netVirtualKey);
};