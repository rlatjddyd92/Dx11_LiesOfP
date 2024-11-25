#pragma once

#include <process.h>

namespace Client
{
	const unsigned int			g_iWinSizeX = { 1280 };
	const unsigned int			g_iWinSizeY = { 720 };

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

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

using namespace Client;
using namespace std;
