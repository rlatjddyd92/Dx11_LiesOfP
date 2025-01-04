#include "stdafx.h"
#include "BloodTrail.h"
#include "GameInstance.h"

#include "Effect_Manager.h"
#include "Trail_Effect_TP.h"

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

	if (FAILED(Ready_Effect(pDesc->eType)))
		return E_FAIL;

	m_pParentMatrix = pDesc->pParentMatrix;
	m_pSocketMatrix = pDesc->pSocketMatrix;

	return S_OK;
}

void CBloodTrail::Priority_Update(_float fTimeDelta)
{
	m_isDead = m_Effect->Get_Dead();
	if (true == m_isDead)
		return;

	m_Effect->Priority_Update(fTimeDelta);
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

	m_Effect->Update(fTimeDelta);
}

void CBloodTrail::Late_Update(_float fTimeDelta)
{
	if (true == m_isDead)
		return;

	m_Effect->Late_Update(fTimeDelta);
}

HRESULT CBloodTrail::Render()
{
	return S_OK;
}

const list<TWOPOINT>& CBloodTrail::Get_TrailPoses()
{
	return m_Effect->Get_TrailPoses();
}

TWOPOINT CBloodTrail::Get_PointPos(_uint iIndex)
{
	return m_Effect->Get_PointPos(iIndex);
}

HRESULT CBloodTrail::Ready_Effect(WEAPON_TYPE eType)
{
	_wstring strEffectTag = TEXT("");
	switch (eType)
	{
	case WEAPON_RAPIER:
		strEffectTag = TEXT("Trail_Player_Attack_Blood_Rapier");
		break;

	case WEAPON_FLAMESWORD:
		strEffectTag = TEXT("Trail_Player_Attack_Blood_FlameSword");
		break;

	case WEAPON_SCISSOR_LEFT:
		strEffectTag = TEXT("Trail_Player_Attack_Blood_Scissor");
		break;

	case WEAPON_SCISSOR_RIGHT:
		strEffectTag = TEXT("Trail_Player_Attack_Blood_Scissor_Right");
		break;
	}

	m_Effect = CEffect_Manager::Get_Instance()->Clone_TrailTP_Effect(strEffectTag);

	if (nullptr == m_Effect)
		return E_FAIL;

	m_Effect->Set_ParentMatrix_Ptr(&m_WorldMatrix);
	m_Effect->Set_Loop(true);

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

	Safe_Release(m_Effect);
}
