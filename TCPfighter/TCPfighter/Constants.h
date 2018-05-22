#pragma once
class Constants
{
public:
	Constants();
	~Constants();

public:
	// 심히 맘에 들지 않아...
	enum class ResourceName {
		MAP, HP, SHADOW,
		Attack1_L_01, Attack1_L_02, Attack1_L_03, Attack1_L_04,
		Attack1_R_01, Attack1_R_02, Attack1_R_03, Attack1_R_04,
		Attack2_L_01, Attack2_L_02, Attack2_L_03, Attack2_L_04,
		Attack2_R_01, Attack2_R_02, Attack2_R_03, Attack2_R_04,
		Attack3_L_01, Attack3_L_02, Attack3_L_03, Attack3_L_04, Attack3_L_05, Attack3_L_06,
		Attack3_R_01, Attack3_R_02, Attack3_R_03, Attack3_R_04, Attack3_R_05, Attack3_R_06,
		Move_L_01, Move_L_02, Move_L_03, Move_L_04, Move_L_05, Move_L_06, Move_L_07, Move_L_08, Move_L_09, Move_L_10, Move_L_11, Move_L_12,
		Move_R_01, Move_R_02, Move_R_03, Move_R_04, Move_R_05, Move_R_06, Move_R_07, Move_R_08, Move_R_09, Move_R_10, Move_R_11, Move_R_12,
		Stand_L_01, Stand_L_02, Stand_L_03, Stand_R_01, Stand_R_02, Stand_R_03,
		xSpark_1, xSpark_2, xSpark_3, xSpark_4,
		Tile_01
	};

public:
	// 구려
	static const int	SERVER_PORT = 20000;
	static const int	RESOURCE_AMOUNTS = 66;
	static const wchar_t*	resourcesFileName[RESOURCE_AMOUNTS];
	static const wchar_t*	resourcesPath;

};