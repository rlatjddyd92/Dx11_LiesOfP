#include "stdafx.h"

#include "GH_Creater.h"
#include "GameInstance.h"

#include "Monster.h"


CGH_Creater::CGH_Creater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CGH_Creater::CGH_Creater(const CGameObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CGH_Creater::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGH_Creater::Initialize(void* pArg)
{
	m_pCopyPlayerTransformCom = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), g_strTransformTag));
		
	return S_OK;
}

void CGH_Creater::Priority_Update(_float fTimeDelta)
{
}

void CGH_Creater::Update(_float fTimeDelta)
{
	if (m_pManus->Get_Dead())	//사망상태인지 확인
	{
		m_isDead = true;
		return;
	}

	m_fStackTime += fTimeDelta;

	if (m_fStackTime >= m_fGodHandsDelay)
	{
		//포지션 땡겨와서 갓핸드 생성
	}

}

void CGH_Creater::Late_Update(_float fTimeDelta)
{
}

HRESULT CGH_Creater::Render()
{
	return E_NOTIMPL;
}

HRESULT CGH_Creater::Render_LightDepth()
{
	return E_NOTIMPL;
}

CGH_Creater* CGH_Creater::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CGH_Creater::Clone(void* pArg)
{
	return nullptr;
}

void CGH_Creater::Free()
{
}
