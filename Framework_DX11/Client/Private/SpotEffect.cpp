#include "stdafx.h"
#include "SpotEffect.h"

#include "GameInstance.h"

#include "Effect_Manager.h"

CSpotEffect::CSpotEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffectObject{ pDevice, pContext }
{
}

CSpotEffect::CSpotEffect(const CGameObject& Prototype)
	: CEffectObject{ Prototype }
{
}

HRESULT CSpotEffect::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CSpotEffect::Initialize(void* pArg)
{
	GAMEOBJECT_DESC Desc{};
	Desc.iLevelIndex = LEVEL_GAMEPLAY;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	EFFECTOBJ_DESC* pDesc = static_cast<EFFECTOBJ_DESC*>(pArg);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);

	m_isLoop = (pDesc->bLoopCheck);

	if (pDesc->vDir.Length() > 0)
	{
		m_pTransformCom->LookAt_Dir(_Vec4{ pDesc->vDir });
	}

	if (FAILED(Ready_Components(pDesc->strEffectTag)))
		return E_FAIL;

	if (m_isLoop)
	{
		m_pEffect->Set_Loop(true);
	}
	else
	{
		m_pEffect->Reset_Effects();
	}
	
	m_bEndCheck = false;

	return S_OK;
}

void CSpotEffect::Priority_Update(_float fTimeDelta)
{
	m_pEffect->Priority_Update(fTimeDelta);
}

void CSpotEffect::Update(_float fTimeDelta)
{
	if (m_fLifeTime >= m_fLifeDuration)
	{
		if (m_isLoop)
		{
			if (!m_bEndCheck)
			{
				m_pEffect->Set_Loop(false);
				m_bEndCheck = true;
			}
			else if (m_pEffect->Get_Dead())
			{
				m_isDead = true;
			}
		}
		else
		{
			if (m_pEffect->Get_Dead())
			{
				m_isDead = true;
			}
		}

	}
	else
	{
		m_fLifeTime += fTimeDelta;
	}

	m_pEffect->Update(fTimeDelta);
}

void CSpotEffect::Late_Update(_float fTimeDelta)
{
	m_pEffect->Late_Update(fTimeDelta);
}

HRESULT CSpotEffect::Ready_Components(_wstring strEffectTag)
{
	const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

	m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(strEffectTag, pParetnMatrix,
		nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	m_pEffect->Set_Loop(true);
	return S_OK;
}

CSpotEffect* CSpotEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpotEffect* pInstance = new CSpotEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSpotEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpotEffect::Clone(void* pArg)
{
	CSpotEffect* pInstance = new CSpotEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSpotEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSpotEffect::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
}
