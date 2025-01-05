#include "stdafx.h"
#include "BloodTrail.h"
#include "GameInstance.h"

#include "Effect_Manager.h"
#include "Trail_Effect_TP.h"

#include "ObjectPool.h"

CBloodTrail::CBloodTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBloodTrail::CBloodTrail(const CBloodTrail& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CBloodTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBloodTrail::Initialize(void* pArg)
{
	BLOODTRAIL_DESC* pDesc = static_cast<BLOODTRAIL_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Effect()))
		return E_FAIL;

	m_eType = pDesc->eStartType;
	m_pParentMatrix = pDesc->pParentMatrix;
	m_pSocketMatrix = pDesc->pSocketMatrix;

	m_isActive = false;

	return S_OK;
}

void CBloodTrail::Priority_Update(_float fTimeDelta)
{
	m_Effect[m_eType]->Priority_Update(fTimeDelta);
}

void CBloodTrail::Update(_float fTimeDelta)
{
	if (true == m_isDead)
		return;

	_matrix ParentMatrix = XMMatrixIdentity();
	_matrix SocketMatrix = XMMatrixIdentity();

	if (nullptr != m_pParentMatrix)
	{
		ParentMatrix = *m_pParentMatrix;
		if (nullptr != m_pSocketMatrix)
		{
			SocketMatrix = *m_pSocketMatrix;
			for (size_t i = 0; i < 3; ++i)
			{
				SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
			}
		}
	}

	m_WorldMatrix = m_pTransformCom->Get_WorldMatrix() * SocketMatrix * ParentMatrix;

	m_Effect[m_eType]->Update(fTimeDelta);
}

void CBloodTrail::Late_Update(_float fTimeDelta)
{
	if (true == m_isDead)
		return;

	m_Effect[m_eType]->Late_Update(fTimeDelta);


	_bool isEffectDead = m_Effect[m_eType]->Get_Dead();
	if (true == isEffectDead)
	{
		m_isActive = false;
		CObjectPool<CBloodTrail>::Return_GameObject(this);
	}
}

HRESULT CBloodTrail::Render()
{
	return S_OK;
}

void CBloodTrail::Set_Weapon(WEAPON_TYPE eType)
{
	m_eType = eType;
	for (size_t i = 0; i < WEAPON_END; ++i)
	{
		if (i == m_eType)
			m_Effect[i]->Set_Loop(true);
		else
			m_Effect[i]->Set_Loop(false);
	}
}

void CBloodTrail::Active(WEAPON_TYPE eType, const _Matrix* pSocketMatrix)
{
	if (m_isActive)
		return;

	m_pSocketMatrix = pSocketMatrix;

	Set_Weapon(eType);
	m_isActive = true;
}

void CBloodTrail::DeActive()
{
}

const list<TWOPOINT>& CBloodTrail::Get_TrailPoses()
{
	return m_Effect[m_eType]->Get_TrailPoses();
}

TWOPOINT CBloodTrail::Get_PointPos(_uint iIndex)
{
	return m_Effect[m_eType]->Get_PointPos(iIndex);
}

HRESULT CBloodTrail::Ready_Effect()
{
	m_Effect[WEAPON_RAPIER] = CEffect_Manager::Get_Instance()->Clone_TrailTP_Effect(TEXT("Trail_Player_Attack_Blood_Rapier"));
	if (nullptr == m_Effect[WEAPON_RAPIER])
		return E_FAIL;
	m_Effect[WEAPON_FLAMESWORD] = CEffect_Manager::Get_Instance()->Clone_TrailTP_Effect(TEXT("Trail_Player_Attack_Blood_FlameSword"));
	if (nullptr == m_Effect[WEAPON_RAPIER])
		return E_FAIL;

	m_Effect[WEAPON_SCISSOR_LEFT] = CEffect_Manager::Get_Instance()->Clone_TrailTP_Effect(TEXT("Trail_Player_Attack_Blood_Scissor"));
	if (nullptr == m_Effect[WEAPON_SCISSOR_LEFT])
		return E_FAIL;

	m_Effect[WEAPON_SCISSOR_RIGHT] = CEffect_Manager::Get_Instance()->Clone_TrailTP_Effect(TEXT("Trail_Player_Attack_Blood_Scissor_Right"));
	if (nullptr == m_Effect[WEAPON_SCISSOR_RIGHT])
		return E_FAIL;

	for (size_t i = 0; i < WEAPON_END; ++i)
	{
		m_Effect[i]->Set_ParentMatrix_Ptr(&m_WorldMatrix);
	}

	return S_OK;
}

CBloodTrail* CBloodTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBloodTrail* pInstance = new CBloodTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CBloodTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBloodTrail::Clone(void* pArg)
{
	CBloodTrail* pInstance = new CBloodTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CBloodTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBloodTrail::Free()
{
	__super::Free();

	for(auto* Effect : m_Effect)
		Safe_Release(Effect);
}
