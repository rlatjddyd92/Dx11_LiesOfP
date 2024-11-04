#pragma once

#include <process.h>

namespace Tools
{
	const unsigned int			g_iWinSizeX = { 1280 };
	const unsigned int			g_iWinSizeY = { 720 };

	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_TOOL, LEVEL_END };
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Tools;
using namespace std;



#ifdef _DEBUG
#undef new
#endif

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "ImGuizmo.h"
#include "GraphEditor.h"
#include "ImSequencer.h"
#include"ImZoomSlider.h"
#include "ImGradient.h"
#include "ImCurveEdit.h"

#ifdef _DEBUG
#define new DBG_NEW
#endif
