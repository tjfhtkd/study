#pragma once

/*
 Camera Widht, Height는 그냥 window size 고정으로 한다.
*/
class Camera
{
public:
	Camera(CGameBase* camBaseTarget);
	Camera(int startPosX, int startPosY, CGameBase* camBaseTarget);
	virtual ~Camera();

public:
	Position CenterPos;
	bool TurnOnFreeCamMode;

public:
	void AdjustCameSpeed(float camSpd);
	void MoveCam(void);
	void SetCameraBaseObject(CGameBase* camBaseTarget);

private:
	bool IsOutPosX(SHORT& x);
	bool IsOutPosY(SHORT& y);

private:
	float m_camSpd;
	CGameBase* m_camBaseTarget;
};