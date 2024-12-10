#pragma once

#pragma warning(disable : 26495) //구조체 경고 끄기

#include <process.h>

namespace Client
{
	const unsigned int			g_iWinSizeX = { 1280 };
	const unsigned int			g_iWinSizeY = { 720 };

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

	enum HIT_TYPE { HIT_CARCASS, HIT_METAL, HIT_ELECTRIC, HIT_FIRE, HIT_END };
	enum ATTACK_STRENGTH { ATK_WEAK, ATK_NORMAL, ATK_STRONG, ATK_END };
	enum CUTSCENE_INDEX {BOSS1_MEET, BOSS1_PHASE2, BOSS1_DEAD, SOPHIA_ENTER, BOSS2_MEET, BOSS2_PHASE2, BOSS2_DEFEAT, SOPHIA_DEAD, BOSS2_MEET2, CUTSCENE_END };

	struct FSM_INIT_DESC
	{
		double* pPrevTrackPos{ nullptr };
	};

	// 24-11-09 김성용 : GameInterface 싱글톤 접근 매크로 
	#define GET_GAMEINTERFACE	CGameInterface_Controller::Get_Instance()
	// 24-11-23 김성용 : UI 텍스쳐 인덱스 최대치
	// 텍스쳐 변동 사항 있으면 반드시 수정할 것 
	#define MAX_TEXTURES_UI 653
	#define MAX_TEXTURES_ITEM 334

	#define NONE_TEXT TEXT("None")

}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

extern float g_fBGMVolume;
extern float g_fEffectVolume;
extern float g_fVoiceVolume;
extern float g_fEnvVolume;
extern float g_fUIVolume;
extern bool g_isHideCursor;

using namespace Client;
using namespace std;
