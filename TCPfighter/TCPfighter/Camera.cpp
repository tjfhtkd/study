#include "stdafx.h"
#include "Camera.h"


Camera::Camera(CGameBase* camBaseTarget)
{
	m_camBaseTarget	= camBaseTarget;
	CenterPos			= camBaseTarget->position;
}

Camera::Camera(int startCenterPosX, int startCenterPosY, CGameBase* camBaseTarget)
{
	TurnOnFreeCamMode	= true;
	CenterPos.X				= startCenterPosX;
	CenterPos.Y				= startCenterPosY;
	m_camBaseTarget		= camBaseTarget;
}

Camera::~Camera()
{
}

void Camera::AdjustCameSpeed(float camSpd)
{
	if (camSpd <= 0)
	{
		camSpd = 0.1;
	}

	if (camSpd > 1.0)
	{
		camSpd = 1.0;
	}

	m_camSpd = camSpd;
}

void Camera::MoveCam(void)
{
	// 자유롭게 프리캠 전환하려고 키 하나는 항상 받게 놔둠.
	if (GetAsyncKeyState(VK_TAB) & 0x8001)
	{
		TurnOnFreeCamMode = !TurnOnFreeCamMode;
	}


	if (TurnOnFreeCamMode == false)
	{
		if (m_camBaseTarget != nullptr)
		{
			if (IsOutPosX(m_camBaseTarget->position.X) == false)
			{
				CenterPos.X = m_camBaseTarget->position.X;
			}

			if (IsOutPosY(m_camBaseTarget->position.Y) == false)
			{
				CenterPos.Y = m_camBaseTarget->position.Y;
			}
		}
		return;
	}

	if (GetAsyncKeyState('W') & 0x8001)
	{
		if (IsOutPosX(CenterPos.X) == false)
		{
			CenterPos.Y -= 100 * m_camSpd;
		}
	}

	if (GetAsyncKeyState('S') & 0x8001)
	{
		if (IsOutPosX(CenterPos.X) == false)
		{
			CenterPos.Y += 100 * m_camSpd;
		}
	}

	if (GetAsyncKeyState('A') & 0x8001)
	{
		if (IsOutPosY(CenterPos.Y) == false)
		{
			CenterPos.X -= 100 * m_camSpd;
		}
	}

	if (GetAsyncKeyState('D') & 0x8001)
	{
		if (IsOutPosY(CenterPos.Y) == false)
		{
			CenterPos.X += 100 * m_camSpd;
		}
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8001)
	{
		if (m_camBaseTarget != nullptr)
		{
			CenterPos.X = m_camBaseTarget->position.X;
			CenterPos.Y = m_camBaseTarget->position.Y;
		}
	}
}

void Camera::SetCameraBaseObject(CGameBase* camBaseTarget)
{
	m_camBaseTarget = camBaseTarget;
}

bool Camera::IsOutPosX(SHORT& x)
{
	int worldWidth	= GameSystemInfo::GetInstance()->GamePlayArea.right;
	int camWidth		= GameSystemInfo::GetInstance()->WindowSize.right;
	
	if (x < camWidth / 2)
	{
		//x = camWidth / 2;
		return true;
	}
	if (x > worldWidth - (camWidth / 2))
	{
		//x = worldWidth - (camWidth / 2);
		return true;
	}
	return false;
}

bool Camera::IsOutPosY(SHORT& y)
{
	int worldHeight	= GameSystemInfo::GetInstance()->GamePlayArea.bottom;
	int camHeight	= GameSystemInfo::GetInstance()->WindowSize.bottom;

	if (y < camHeight / 2)
	{
		//y = camHeight / 2;
		return true;
	}
	if (y > worldHeight - (camHeight / 2))
	{
		//y = worldHeight - (camHeight / 2);
		return true;
	}
	return false;
}
