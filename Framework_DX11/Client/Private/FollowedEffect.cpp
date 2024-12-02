#include "stdafx.h"
#include "FollowedEffect.h"

#include "GameInstance.h"

#include "Effect_Manager.h"

CFollowedEffect::CFollowedEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffectObject{ pDevice, pContext }
{
}

CFollowedEffect::CFollowedEffect(const CGameObject& Prototype)
	: CEffectObject{ Prototype }
{
}

HRESULT CFollowedEffect::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CFollowedEffect::Initialize(void* pArg)
{
	GAMEOBJECT_DESC Desc{};
	Desc.iLevelIndex = LEVEL_GAMEPLAY;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	FOLLOWEFFOBJ_DESC* pDesc = static_cast<FOLLOWEFFOBJ_DESC*>(pArg);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

	if (pDesc->vDir.Length() > 0)
	{
		m_pTransformCom->LookAt_Dir(_Vec4{ pDesc->vDir });
	}

	if (FAILED(Ready_Components(pDesc->strEffectTag)))
		return E_FAIL;

	m_pEffect->Set_Loop(true);

	if (pDesc->pParentWorldMat == nullptr)
		return E_FAIL;

	m_pParentWorldMat = pDesc->pParentWorldMat;
	m_pBoneCombinedMat = pDesc->pBoneCombinedMat;

	return S_OK;
}

void CFollowedEffect::Priority_Update(_float fTimeDelta)
{
	m_pEffect->Priority_Update(fTimeDelta);
}

void CFollowedEffect::Update(_float fTimeDelta)
{
	_float4x4 WorldMat{};
	XMStoreFloat4x4(&WorldMat, ((*m_pBoneCombinedMat) * (*m_pParentWorldMat)));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _Vec3{ WorldMat._41, WorldMat._42, WorldMat._43 });


	if (m_fLifeTime >= m_fLifeDuration)
	{
		if (m_pEffect->Get_Active())
		{
			m_pEffect->Set_Loop(false);
		}
		else if (m_pEffect->Get_Dead())
		{
			m_isDead = true;
		}
	}
	else
	{
		m_fLifeTime += fTimeDelta;
	}

	m_pEffect->Update(fTimeDelta);
}

void CFollowedEffect::Late_Update(_float fTimeDelta)
{
	m_pEffect->Late_Update(fTimeDelta);
}

HRESULT CFollowedEffect::Ready_Components(_wstring strEffectTag)
{
	const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

	m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(strEffectTag, pParetnMatrix,
		nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	m_pEffect->Set_Loop(true);
	return S_OK;
}

CFollowedEffect* CFollowedEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFollowedEffect* pInstance = new CFollowedEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpotEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFollowedEffect::Clone(void* pArg)
{
	CFollowedEffect* pInstance = new CFollowedEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CFollowedEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFollowedEffect::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
}
