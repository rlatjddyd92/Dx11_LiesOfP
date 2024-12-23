#pragma once

#pragma warning(disable : 26495) //±∏¡∂√º ∞Ê∞Ì ≤Ù±‚

#include <process.h>

namespace Client
{
	const unsigned int			g_iWinSizeX = { 1280 };
	const unsigned int			g_iWinSizeY = { 720 };

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

	enum HIT_TYPE { HIT_CARCASS, HIT_METAL, HIT_ELECTRIC, HIT_FIRE, HIT_END };
	enum ATTACK_STRENGTH { ATK_WEAK, ATK_NORMAL, ATK_STRONG, ATK_LAST, ATK_END };
	enum CUTSCENE_INDEX {BOSS1_MEET1, BOSS1_PHASE2, BOSS1_DEAD, SOPHIA_ENTER, BOSS2_MEET, BOSS2_PHASE2, BOSS2_DEFEAT, SOPHIA_DEAD, BOSS2_MEET2, BOSS2_MEET3, BOSS1_MEET2, CUTSCENE_END };

	struct FSM_INIT_DESC
	{
		double* pPrevTrackPos{ nullptr };
	};

	// 24-11-09 ±Ëº∫øÎ : GameInterface ΩÃ±€≈Ê ¡¢±Ÿ ∏≈≈©∑Œ 
	#define GET_GAMEINTERFACE	CGameInterface_Controller::Get_Instance()

	#define NONE_TEXT TEXT("None")

}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

extern float g_fBGMVolume;
extern float g_fEffectVolume;
extern float g_fVoiceVolume;
extern float g_fEnvVolume;
extern float g_fUIVolume;
extern float g_fCutSceneVolume;
extern bool g_isHideCursor;

using namespace Client;
using namespace std;
