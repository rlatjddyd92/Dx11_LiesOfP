#include "stdafx.h"
#include "Weapon_Scissor_Handle.h"
#include "Player.h"
#include "Weapon_Scissor_Blade.h"

#include "GameInstance.h"

CWeapon_Scissor_Handle::CWeapon_Scissor_Handle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_Scissor_Handle::CWeapon_Scissor_Handle(const CWeapon_Scissor_Handle& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CWeapon_Scissor_Handle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Scissor_Handle::Initialize(void* pArg)
{
	SCISSOR_DESC* pDesc = static_cast<SCISSOR_DESC*>(pArg);
	m_eType = pDesc->eScissorType;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Blade()))
		return E_FAIL;

	m_strObjectTag = TEXT("PlayerWeapon");
	m_fDamageAmount = 5.f;

	m_isActive = false;
	m_pColliderCom->IsActive(false);

	return S_OK;
}

void CWeapon_Scissor_Handle::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;
}

void CWeapon_Scissor_Handle::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;


	__super::Update(fTimeDelta);

	m_pBlade->Update(fTimeDelta);

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CWeapon_Scissor_Handle::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;


	m_pBlade->Late_Update(fTimeDelta);


#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);

#endif
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

}

HRESULT CWeapon_Scissor_Handle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Scissor_Handle::Render_LightDepth()
{
	if (FAILED(__super::Render_LightDepth()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Scissor_Handle::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (m_eType == SCISSOR_LEFT)
	{
		/* FOR.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Scissor_Left_Hnd"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

		/* FOR.Com_Collider */
		CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
		ColliderDesc.vExtents = _float3(0.1f, 0.1f, 0.75f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, -0.6f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;
	}
	else if (m_eType == SCISSOR_RIGHT)
	{
		/* FOR.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Scissor_Right_Hnd"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

		/* FOR.Com_Collider */
		CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
		ColliderDesc.vExtents = _float3(0.1f, 0.1f, 0.75f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, 0.6f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;
	}




	return S_OK;
}

HRESULT CWeapon_Scissor_Handle::Ready_Blade()
{
	CWeapon_Scissor_Handle::SCISSOR_DESC ScissorDesc;
	ScissorDesc.pParentWorldMatrix = &m_WorldMatrix;
	ScissorDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Blade");

	if (m_eType == SCISSOR_LEFT)
	{
		ScissorDesc.eScissorType = SCISSOR_LEFT;
		m_pBlade = dynamic_cast<CWeapon_Scissor_Blade*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Scissor_Blade"), &ScissorDesc));
	}
	else if (m_eType == SCISSOR_RIGHT)
	{
		ScissorDesc.eScissorType = SCISSOR_RIGHT;
		m_pBlade = dynamic_cast<CWeapon_Scissor_Blade*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Scissor_Blade"), &ScissorDesc));
		if (nullptr == m_pBlade)
			return E_FAIL;
	}

	if (nullptr == m_pBlade)
		return E_FAIL;

	return S_OK;
}

CWeapon_Scissor_Handle* CWeapon_Scissor_Handle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Scissor_Handle* pInstance = new CWeapon_Scissor_Handle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Scissor_Handle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_Scissor_Handle::Clone(void* pArg)
{
	CWeapon_Scissor_Handle* pInstance = new CWeapon_Scissor_Handle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Scissor_Handle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Scissor_Handle::Free()
{
	__super::Free();

	Safe_Release(m_pBlade);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
