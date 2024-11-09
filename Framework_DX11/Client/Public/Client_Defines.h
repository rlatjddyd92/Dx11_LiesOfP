#pragma once

#include <process.h>

namespace Client
{
	const unsigned int			g_iWinSizeX = { 1280 };
	const unsigned int			g_iWinSizeY = { 720 };

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

	// 24-11-09 ±Ëº∫øÎ : GameInterface ΩÃ±€≈Ê ¡¢±Ÿ ∏≈≈©∑Œ 
	#define GET_GAMEINTERFACE	CGameInterface_Controller::Get_Instance()
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
using namespace std;
