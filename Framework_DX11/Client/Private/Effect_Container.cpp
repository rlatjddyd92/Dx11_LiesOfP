#include "stdafx.h"
#include "Effect_Container.h"
#include "GameInstance.h"

#include "Particle_Effect.h"
#include "Texture_Effect.h"


CEffect_Container::CEffect_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect_Container::CEffect_Container(const CEffect_Container& Prototype)
	: CGameObject(Prototype)
{
}

HRESULT CEffect_Container::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Container::Initialize(void* pArg)
{
	EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// 위치 회전이랑 크기 받아올수 잇게
	m_WorldMatrix = XMMatrixIdentity();
	m_pParentMatrix = pDesc->pParentMatrix;
	m_pSocketMatrix = pDesc->pSocketMatrix;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPos);
	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);

	if(0.f < pDesc->vDir.Length())
		m_pTransformCom->LookAt(_Vec4(pDesc->vDir.x, pDesc->vDir.y, pDesc->vDir.z, 0.f));

	return S_OK;
}

void CEffect_Container::Priority_Update(_float fTimeDelta)
{
	for (auto& Effect : m_Effects)
	{
		if (nullptr == Effect)
			continue;

		if (false == Effect->Get_Dead())
			Effect->Priority_Update(fTimeDelta);
	}
}

void CEffect_Container::Update(_float fTimeDelta)
{
	_matrix ParentMatrix = XMMatrixIdentity();
	_matrix SocketMatrix = XMMatrixIdentity();

	if(nullptr != m_pParentMatrix)
	{
		ParentMatrix = *m_pParentMatrix;
		if(nullptr != m_pSocketMatrix)
		{
			SocketMatrix = *m_pSocketMatrix;
			for (size_t i = 0; i < 3; ++i)
			{
				SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
			}
		}
	}

	if (KEY_TAP(KEY::H))
	{
		m_bTurn = !m_bTurn;
	}

	if (KEY_TAP(KEY::J))
		Set_Loop(true);


	if (KEY_TAP(KEY::K))
		Set_Loop(false);

	if (true == m_bTurn)
	{
		m_pTransformCom->Turn(_Vec4(0.f, 1.f, 0.f, 0.f), fTimeDelta, 10.f);
	}

	m_WorldMatrix = m_pTransformCom->Get_WorldMatrix() * SocketMatrix * ParentMatrix;

	for (auto& Effect : m_Effects)
	{
		if (nullptr == Effect)
			continue;

		if(false== Effect->Get_Dead())
			Effect->Update(fTimeDelta);
	}
}

void CEffect_Container::Late_Update(_float fTimeDelta)
{
	m_isDead = { true };
	for (auto& Effect : m_Effects)
	{
		if (nullptr == Effect)
			continue;
	
		if(false == Effect->Get_Dead())
		{
			Effect->Late_Update(fTimeDelta);
			m_isDead = false;
		}
	}

	if (true == m_isDead)
		int a = 0;
}

HRESULT CEffect_Container::Render()
{

	return S_OK;
}

HRESULT CEffect_Container::Add_Effect(class CEffect_Base* pEffectBase)
{
	pEffectBase->Set_ParentMatrix_Ptr(&m_WorldMatrix);
	m_Effects.emplace_back(pEffectBase);

	return S_OK;
}


HRESULT CEffect_Container::Reset_Effects()
{
	for (auto& Effect : m_Effects)
	{
		Effect->Reset();
	}

	return S_OK;
}

void CEffect_Container::Set_Loop(_bool bLoop)
{
	if (true == bLoop)
		m_isDead = false;

	m_bActive = bLoop;

	for (auto& Effect : m_Effects)
	{
		Effect->Set_Loop(bLoop);
	}
}

void CEffect_Container::Set_EffectDesc(const EFFECT_DESC& desc)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, desc.vPos);
	m_pTransformCom->Set_Scaled(desc.vScale.x, desc.vScale.y, desc.vScale.z);
	m_pTransformCom->Rotation(desc.vDir.x, desc.vDir.y, desc.vDir.z);

	m_pParentMatrix = desc.pParentMatrix;
	m_pSocketMatrix = desc.pSocketMatrix;

	Reset_Effects();
}

CEffect_Container* CEffect_Container::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Container* pInstance = new CEffect_Container(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CEffect_Container"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Container::Clone(void* pArg)
{
	CEffect_Container* pInstance = new CEffect_Container(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CEffect_Container"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Container::Free()
{
	__super::Free();

	for (auto& elem : m_Effects)
		Safe_Release(elem);
	
	m_Effects.clear();
}
