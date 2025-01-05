#include "stdafx.h"
#include"PlayerCollider_Fatal.h"

#include "GameInstance.h"
#include "ColliderObject.h"

#include "Player.h"
#include "Monster.h"

CPlayerCollider_Fatal::CPlayerCollider_Fatal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPlayerCollider_Fatal::CPlayerCollider_Fatal(const CPlayerCollider_Fatal& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CPlayerCollider_Fatal::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerCollider_Fatal::Initialize(void* pArg)
{
	FATALCOLLIDER_DESC* pDesc = static_cast<FATALCOLLIDER_DESC*>(pArg);

	m_pPlayer = pDesc->pPlayer;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pParentMatrix = m_pPlayer->Get_Transform()->Get_WorldMatrix_Ptr();
	m_pSocketMatrix = m_pPlayer->Get_Model()->Get_BoneCombindTransformationMatrix_Ptr("Bip001-Spine");

	m_pColliderCom->IsActive(true);
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

void CPlayerCollider_Fatal::Priority_Update(_float fTimeDelta)
{
	if (m_pContactMonster != nullptr)
	{
		if (m_pContactMonster->Get_IsDieState() || m_pContactMonster->Get_Dead())
		{
			m_pContactMonster = nullptr;
		}
	}
}

void CPlayerCollider_Fatal::Update(_float fTimeDelta)
{
}

void CPlayerCollider_Fatal::Late_Update(_float fTimeDelta)
{
	//if (!m_pColliderCom->IsActive())
	//	return;

	_float4x4 WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, (*m_pSocketMatrix) * (*m_pParentMatrix));

	m_pColliderCom->Update(&WorldMatrix);

	if (nullptr != m_pColliderCom)
		m_pGameInstance->Add_ColliderList(m_pColliderCom);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPlayerCollider_Fatal::Render()
{

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

void CPlayerCollider_Fatal::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Monster"))
	{
		m_pContactMonster = dynamic_cast<CMonster*>(pOther);
		if (m_pContactMonster->Get_IsDieState())
		{
			m_pContactMonster = nullptr;
		}
	}
}

void CPlayerCollider_Fatal::OnCollisionStay(CGameObject* pOther)
{
}

void CPlayerCollider_Fatal::OnCollisionExit(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Monster"))
	{
		m_pContactMonster = dynamic_cast<CMonster*>(pOther);
	}
}

HRESULT CPlayerCollider_Fatal::Ready_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC ColliderSphereDesc{};
	ColliderSphereDesc.fRadius = 1.2f;
	ColliderSphereDesc.vCenter = _Vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Sphere"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderSphereDesc)))
		return E_FAIL;

	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CPlayerCollider_Fatal* CPlayerCollider_Fatal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CPlayerCollider_Fatal* pInstance = new CPlayerCollider_Fatal(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayerCollider_Fatal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerCollider_Fatal::Clone(void* pArg)
{
	CPlayerCollider_Fatal* pInstance = new CPlayerCollider_Fatal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayerCollider_Fatal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerCollider_Fatal::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
