#include "stdafx.h"
#include "Trail_Effect_TP.h"
#include "GameInstance.h"

CTrail_Effect_TP::CTrail_Effect_TP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base(pDevice, pContext)
{
}

CTrail_Effect_TP::CTrail_Effect_TP(const CTrail_Effect_TP& Prototype)
	: CEffect_Base(Prototype)
{
}

HRESULT CTrail_Effect_TP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrail_Effect_TP::Initialize(void* pArg)
{
	TRAIL_TP_DESC* pDesc = static_cast<TRAIL_TP_DESC*>(pArg);
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	m_ActionDesc = pDesc->InitDesc.ActionDesc;
	m_SaveDesc = *pDesc;

	return S_OK;
}

void CTrail_Effect_TP::Priority_Update(_float fTimeDelta)
{
}

void CTrail_Effect_TP::Update(_float fTimeDelta)
{
}

void CTrail_Effect_TP::Late_Update(_float fTimeDelta)
{
}

HRESULT CTrail_Effect_TP::Render()
{
	return S_OK;
}

void CTrail_Effect_TP::Reset()
{
	m_pVIBufferCom->Reset();
}

HRESULT CTrail_Effect_TP::Save(_wstring strFilePath)
{
	return S_OK;
}

void CTrail_Effect_TP::Set_Desc(const ACTION_DESC& desc)
{
	m_ActionDesc = desc;
	m_SaveDesc.InitDesc.ActionDesc = desc;
}

CTrail_Effect_TP::TRAIL_TP_DESC CTrail_Effect_TP::Get_Desc()
{
	TRAIL_TP_DESC desc = {};
	desc.BufferDesc = m_SaveDesc.BufferDesc;
	desc.InitDesc.ActionDesc = m_ActionDesc;
	return desc;

}

HRESULT CTrail_Effect_TP::Ready_Components(const TRAIL_TP_DESC& Desc)
{
	return S_OK;
}

CTrail_Effect_TP* CTrail_Effect_TP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrail_Effect_TP* pInstance = new CTrail_Effect_TP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CTrail_Effect_TP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrail_Effect_TP::Clone(void* pArg)
{
	CTrail_Effect_TP* pInstance = new CTrail_Effect_TP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CTrail_Effect_TP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_Effect_TP::Free()
{
	__super::Free();

	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pMaskTextureCom_1);
	Safe_Release(m_pMaskTextureCom_2);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
