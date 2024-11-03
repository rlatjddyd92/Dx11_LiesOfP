#include "stdafx.h"
#include "Controller_AnimationTool.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CController_AnimationTool)

CController_AnimationTool::CController_AnimationTool()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CController_AnimationTool::Initialize()
{
	return S_OK;
}

void CController_AnimationTool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
