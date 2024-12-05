#include "stdafx.h"

#include "GH_Creater.h"
#include "GameInstance.h"

#include "Monster.h"
#include "AttackObject.h"



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
	CREATER_DESC* pDesc = static_cast<CREATER_DESC*>(pArg);
	m_pManus = pDesc->pManus;
	m_pCopyPlayerTransformCom = static_cast<CTransform*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), g_strTransformTag));
	m_fGodHandsDelay = 5.f;
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
		m_fStackTime = 0.f;
		m_fGodHandsDelay = 15.f;
		//포지션 땡겨와서 갓핸드 생성

		CAttackObject::ATKOBJ_DESC Desc;
		Desc.vPos = _Vec3{ m_pCopyPlayerTransformCom->Get_State(CTransform::STATE_POSITION)};

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Attack"), TEXT("Prototype_GameObject_GodHands"), &Desc);

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
	CGH_Creater* pInstance = new CGH_Creater(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CGH_Creater"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGH_Creater::Clone(void* pArg)
{
	CGH_Creater* pInstance = new CGH_Creater(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CGH_Creater"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGH_Creater::Free()
{
	__super::Free();
}
