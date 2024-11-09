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

	for(auto& elem : pDesc->pParticleEffect_Descs)
	{
		if (nullptr == elem)
			continue;

		CParticle_Effect::PARTICLE_EFFECT_DESC* pParticleDesc = static_cast<CParticle_Effect::PARTICLE_EFFECT_DESC*>(elem);
		pParticleDesc->pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		m_Effects.emplace_back(static_cast<CEffect_Base*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle_Effect"), pParticleDesc)));
	}

	for (auto& elem : pDesc->pTextureEffect_Descs)
	{
		if (nullptr == elem)
			continue;

		CTexture_Effect::TEXTURE_EFFECT_DESC* pTextureDesc = static_cast<CTexture_Effect::TEXTURE_EFFECT_DESC*>(elem);
		pTextureDesc->pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		m_Effects.emplace_back(static_cast<CEffect_Base*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Texture_Effect"), pTextureDesc)));
	}

	for (auto& elem : pDesc->pMeshEffect_Descs)
	{
		if (nullptr == elem)
			continue;
	}


	return S_OK;
}

void CEffect_Container::Priority_Update(_float fTimeDelta)
{
	for (auto& Effect : m_Effects)
	{
		if (nullptr == Effect)
			continue;

		Effect->Priority_Update(fTimeDelta);
	}
}

void CEffect_Container::Update(_float fTimeDelta)
{
	

	for (auto& Effect : m_Effects)
	{
		if (nullptr == Effect)
			continue;

		Effect->Update(fTimeDelta);
	}
}

void CEffect_Container::Late_Update(_float fTimeDelta)
{
	for (auto& Effect : m_Effects)
	{
		if (nullptr == Effect)
			continue;

		Effect->Late_Update(fTimeDelta);
	}
}

HRESULT CEffect_Container::Render()
{

	return S_OK;
}

HRESULT CEffect_Container::Save_Effects(_wstring strFilePath)
{
	for (auto& Effect : m_Effects)
	{
		if (nullptr == Effect)
			continue;

		if(FAILED(Effect->Save(strFilePath)))
			return E_FAIL;
	}

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
