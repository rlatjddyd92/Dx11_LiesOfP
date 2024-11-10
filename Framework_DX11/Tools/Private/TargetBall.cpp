#include "stdafx.h"
#include "../Public/TargetBall.h"

#include "GameInstance.h"



CTargetBall::CTargetBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEasyDrawSphere{ pDevice, pContext }
{
}

CTargetBall::CTargetBall(const CTargetBall& Prototype)
	: CEasyDrawSphere{ Prototype }
{
}

HRESULT CTargetBall::Initialize_Prototype()
{	
	if (FAILED(__super::Initialize_Prototype()))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CTargetBall::Initialize(void* pArg)
{
	TARGETBALL_DESC* pDesc = static_cast<TARGETBALL_DESC*>(pArg);
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	memcpy(&m_tDesc, pDesc, sizeof(TARGETBALL_DESC));
	if (m_tDesc.pUpdateCtr == nullptr)
		return E_FAIL;

	return S_OK;
}

void CTargetBall::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CTargetBall::Update(_float fTimeDelta)
{
	if (*(m_tDesc.pUpdateCtr) == false)
	{
		return;
	}
	__super::Update(fTimeDelta);
}

void CTargetBall::Late_Update(_float fTimeDelta)
{
	if (*(m_tDesc.pUpdateCtr) == false)
	{
		return;
	}

	__super::Late_Update(fTimeDelta);
	
	if (*(m_tDesc.pRenderCtr) == true)
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this);
	}
}

HRESULT CTargetBall::Render()
{
	if (FAILED(__super::Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}


CTargetBall* CTargetBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTargetBall* pInstance = new CTargetBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTargetBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CTargetBall::Clone(void* pArg)
{
	CTargetBall* pInstance = new CTargetBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTargetBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTargetBall::Free()
{
	__super::Free();

}
