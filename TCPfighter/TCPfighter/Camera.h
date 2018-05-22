#pragma once

/*
 Camera Widht, Height�� �׳� window size �������� �Ѵ�.
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