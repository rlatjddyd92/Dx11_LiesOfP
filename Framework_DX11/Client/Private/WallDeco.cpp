#include "stdafx.h"
#include "WallDeco.h"
#include "GameInstance.h"
#include "Deco_Collider.h"
CWallDeco::CWallDeco(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CWallDeco::CWallDeco(const CWallDeco& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CWallDeco::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWallDeco::Initialize(void* pArg)
{
	OBJECT_DEFAULT_DESC* pDesc = static_cast<OBJECT_DEFAULT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	 
	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(0.f, pDesc->vRotation.y+90.f, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	m_bShadow = pDesc->bShadow;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iAnim_Activate = m_pModelCom->Find_AnimationIndex("AS_Activate", 1.f);
	m_iAnim_Deactivate = m_pModelCom->Find_AnimationIndex("AS_Deactivate", 1.f);

	m_pModelCom->SetUp_Animation(m_iAnim_Deactivate, true);

	m_strObjectTag = TEXT("WallDeco");

	CDeco_Collider::DECO_COLLIDER_DESC Desc = {};
	Desc.pSoketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(3);
	Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

	CGameObject* pColliderObj = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Deco_Collider"), &Desc);
	if (nullptr != pColliderObj)
	{
		m_pCollider_Object = pColliderObj;
	}

	return S_OK;
}

void CWallDeco::Priority_Update(_float fTimeDelta)
{
	if(m_pCollider_Object != nullptr)
		m_pCollider_Object->Priority_Update(fTimeDelta);
}

void CWallDeco::Update(_float fTimeDelta)
{
	if(m_bDetect)
	{
		if(m_pModelCom->Get_CurrentAnimationIndex() == m_iAnim_Deactivate)
			m_pModelCom->SetUp_NextAnimation(m_iAnim_Activate, false);
	}
	
	if (m_fPlayActiveAnimTimer > 5.f
		&& m_pModelCom->Get_IsEndAnimArray())
	{
		m_fPlayActiveAnimTimer = 0.f;
		m_pModelCom->SetUp_NextAnimation(m_iAnim_Deactivate, true);
	}

	if (m_pModelCom->Get_CurrentAnimationIndex() == m_iAnim_Activate)
	{
		m_fPlayActiveAnimTimer += fTimeDelta;
	}

	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_pColliderCom != nullptr)
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

	if (m_pCollider_Object != nullptr)
		m_pCollider_Object->Update(fTimeDelta);
}

void CWallDeco::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_ColliderList(m_pColliderCom);

	if (m_pCollider_Object != nullptr)
		m_pCollider_Object->Late_Update(fTimeDelta);

#ifdef _DEBUG
	if (m_pColliderCom != nullptr)
		m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CWallDeco::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::ROUGHNESS))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ARMTexture", ROUGHNESS, (_uint)i)))
				return E_FAIL;
		}

		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::NORMALS))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", NORMALS, (_uint)i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;

}

void CWallDeco::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Player"))
	{
		m_bDetect = true;
	}
}

void CWallDeco::OnCollisionStay(CGameObject* pOther)
{
}

void CWallDeco::OnCollisionExit(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Player"))
	{
		m_bDetect = false;
	}
}

HRESULT CWallDeco::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_WallDeco"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(3.f, 1.f, 1.8f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vCenter = _float3(2.5f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CWallDeco* CWallDeco::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWallDeco* pInstance = new CWallDeco(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWallDeco"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWallDeco::Clone(void* pArg)
{
	CWallDeco* pInstance = new CWallDeco(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWallDeco"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWallDeco::Free()
{
	__super::Free();
	Safe_Release(m_pCollider_Object);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
