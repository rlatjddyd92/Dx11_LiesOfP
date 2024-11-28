#include "stdafx.h"
#include "Lift_Door.h"
#include "GameInstance.h"

CLift_Door::CLift_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLift_Door::CLift_Door(const CLift_Door& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CLift_Door::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLift_Door::Initialize(void* pArg)
{
	OBJECT_DEFAULT_DESC* pDesc = static_cast<OBJECT_DEFAULT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(0.f, -25.f,0.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	m_bShadow = pDesc->bShadow;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_iAnim_Close = m_pModelCom->Find_AnimationIndex("AS_Close", 3.f);
	m_iAnim_Close_Idle = m_pModelCom->Find_AnimationIndex("AS_Close_Idle", 3.f);
	m_iAnim_Open = m_pModelCom->Find_AnimationIndex("AS_Open", 3.f);
	m_iAnim_Open_Idle = m_pModelCom->Find_AnimationIndex("AS_Open_Idle", 3.f);

	m_pModelCom->SetUp_Animation(m_iAnim_Close_Idle, false);

	return S_OK;
}

void CLift_Door::Priority_Update(_float fTimeDelta)
{
}

void CLift_Door::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_bOpen)
	{
		if(m_pModelCom->Get_CurrentAnimationIndex() != m_iAnim_Open
			&& m_pModelCom->Get_CurrentAnimationIndex() == m_iAnim_Close_Idle)
			m_pModelCom->SetUp_NextAnimation(m_iAnim_Open);

		if(m_pModelCom->Get_CurrentAnimationIndex() == m_iAnim_Open 
			&& m_pModelCom->Get_IsEndAnimArray())
			m_pModelCom->SetUp_NextAnimation(m_iAnim_Open_Idle);
	}
	
	if(m_bClose)
	{
		if (m_pModelCom->Get_CurrentAnimationIndex() == m_iAnim_Open_Idle)
			m_pModelCom->SetUp_NextAnimation(m_iAnim_Close);

		if (m_pModelCom->Get_CurrentAnimationIndex() == m_iAnim_Close
			&& m_pModelCom->Get_IsEndAnimArray()
			&& m_pModelCom->Get_CurrentAnimationIndex() != m_iAnim_Close_Idle)
			m_pModelCom->SetUp_NextAnimation(m_iAnim_Close_Idle);
	}

	if(m_pColliderCom != nullptr)
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CLift_Door::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
	if (m_pColliderCom != nullptr)
		m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
}

HRESULT CLift_Door::Render()
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

HRESULT CLift_Door::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lift_Door"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(1.8f, 1.0f, 0.2f);
	ColliderDesc.vAngles = _float3(0.f, XMConvertToRadians(-25.f), 0.f);
	ColliderDesc.vCenter = _float3(0.f, 1.0f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

CLift_Door* CLift_Door::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLift_Door* pInstance = new CLift_Door(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLift_Door"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLift_Door::Clone(void* pArg)
{
	CLift_Door* pInstance = new CLift_Door(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLift_Door"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLift_Door::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
