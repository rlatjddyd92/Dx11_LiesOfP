 #include "stdafx.h"
#include "Ladder.h"
#include "GameInstance.h"

CLadder::CLadder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLadder::CLadder(const CLadder& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CLadder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLadder::Initialize(void* pArg)
{
	LADDER_DESC* pDesc = static_cast<LADDER_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	if (pDesc->vRotation.y < 65.f)
		pDesc->vRotation.y -= 90.f;
	else
		pDesc->vRotation.y += 90.f;
	m_pTransformCom->Rotation(0.f, pDesc->vRotation.y, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	m_bShadow = pDesc->bShadow;
	m_iCurrentAreaNum = pDesc->iCurrentAreaNum;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	int iAnimIndex = m_pModelCom->Find_AnimationIndex("AS_Down_Idle", 3.f);

	m_pModelCom->SetUp_Animation(iAnimIndex, true);

	m_strLadderTag = pDesc->szModelTag;

	m_strObjectTag = TEXT("Ladder");

	return S_OK;
}

void CLadder::Priority_Update(_float fTimeDelta)
{
}

void CLadder::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_pColliderCom != nullptr)
	{
		m_pColliderCom[UP]->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
		m_pColliderCom[DOWN]->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
	}
}

void CLadder::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	m_pGameInstance->Add_ColliderList(m_pColliderCom[UP]);
	m_pGameInstance->Add_ColliderList(m_pColliderCom[DOWN]);


#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom[UP]);
	m_pGameInstance->Add_DebugObject(m_pColliderCom[DOWN]);
#endif
}

HRESULT CLadder::Render()
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

const _Vec3 CLadder::Get_LadderUpPos()
{
	return m_pColliderCom[UP]->Get_WorldCenter();
}

const _Vec3 CLadder::Get_LadderDonwPos()
{
	return m_pColliderCom[DOWN]->Get_WorldCenter();
}

HRESULT CLadder::Ready_Components(LADDER_DESC* Desc)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	_tchar szModelTag[MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, Desc->szModelTag, MAX_PATH, szModelTag, MAX_PATH);

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, szModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(1.f, 0.1f, 1.f);
	ColliderDesc.vAngles = _float3(0.f,0.f, 0.f);

	if (strcmp(Desc->szModelTag, "SK_LV_Ladder_MetalWood_Slide6m_SM_KSJ") == 0)	//ÂªÀº³ð
	{
		ColliderDesc.vCenter = _float3(0.f, 0.5f, 0.52f);
	}
	else
	{
		ColliderDesc.vCenter = _float3(0.f, 1.f, 0.52f);
	}


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider1"), reinterpret_cast<CComponent**>(&m_pColliderCom[DOWN]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom[DOWN]->Set_Owner(this);
	m_pColliderCom[DOWN]->IsActive(true);

	if (strcmp(Desc->szModelTag, "SK_LV_Ladder_MetalWood_Slide6m_SM_KSJ") == 0)	//ÂªÀº³ð
	{
		ColliderDesc.vExtents = _float3(1.f, 0.3f, 1.f);
		ColliderDesc.vCenter = _float3(0.f, 5.8f, 0.52f);
	}
	else
	{
		ColliderDesc.vExtents = _float3(1.f, 0.3f, 1.f);	//±ä³ð
		ColliderDesc.vCenter = _float3(0.f, 9.8f, 0.52f);
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider2"), reinterpret_cast<CComponent**>(&m_pColliderCom[UP]), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom[UP]->Set_Owner(this);
	m_pColliderCom[UP]->IsActive(true);

	return S_OK;
}

CLadder* CLadder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLadder* pInstance = new CLadder(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLadder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLadder::Clone(void* pArg)
{
	CLadder* pInstance = new CLadder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLadder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLadder::Free()
{
	__super::Free();
	for (_uint i = 0; i < COLLIDER_END; ++i)
	{
		Safe_Release(m_pColliderCom[i]);
	}
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
