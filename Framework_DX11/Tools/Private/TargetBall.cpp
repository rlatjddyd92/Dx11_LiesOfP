#include "stdafx.h"
#include "TargetBall.h"

#include "GameInstance.h"
#include "ImGui_Manager.h"

#include "Controller_EffectTool.h"

CTargetBall::CTargetBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CTargetBall::CTargetBall(const CTargetBall& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CTargetBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTargetBall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTargetBall::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void CTargetBall::Update(_float fTimeDelta)
{


}

void CTargetBall::Late_Update(_float fTimeDelta)
{
	if (*(m_pUpdateCtr) == false)
	{
		return;
	}
	__super::Late_Update(fTimeDelta);
	

	m_pTargetPos;
	
	if(false == CController_EffectTool::Get_Instance()->Get_JunhoCamera())
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
}

HRESULT CTargetBall::Render()
{

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ));

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	//m_pBounding->Render(m_pBatch);

	m_pBatch->End();

	return S_OK;
}

HRESULT CTargetBall::Ready_Components()
{

	return S_OK;
}


CTargetBall* CTargetBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTargetBall* pInstance = new CTargetBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CTargetBall::Clone(void* pArg)
{
	CTargetBall* pInstance = new CTargetBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTargetBall::Free()
{
	__super::Free();

}
