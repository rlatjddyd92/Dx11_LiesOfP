#include "stdafx.h"
#include "Controller_3DUITool.h"
#include "GameInstance.h"


IMPLEMENT_SINGLETON(CController_3DUITool)

HRESULT CController_3DUITool::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;


	return S_OK;
}

void CController_3DUITool::Edit_Guidline()
{
}

void CController_3DUITool::Render_Guidline()
{
}

void CController_3DUITool::SelectNavi()
{
}

void CController_3DUITool::SelectNode()
{
}

void CController_3DUITool::Adjust_Pos()
{
}

void CController_3DUITool::ConnectingNode()
{
}

CController_3DUITool::CController_3DUITool()
{
}

void CController_3DUITool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
