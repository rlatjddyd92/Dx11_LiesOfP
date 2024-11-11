#include "stdafx.h"
#include "NonAnimModel.h"

#include "GameInstance.h"
#include "ImGui_Manager.h"

_int CNonAnimModel::m_iStaticHashId = 0;

CNonAnimModel::CNonAnimModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CNonAnimModel::CNonAnimModel(const CNonAnimModel& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CNonAnimModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNonAnimModel::Initialize(void* pArg)
{
	NONMODEL_DESC* pDesc = static_cast<NONMODEL_DESC*>(pArg);

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	m_iRenderGroupId = pDesc->iRenderGroupID;
	m_isLight = pDesc->isLight;
	m_isInstance = pDesc->isInstance;
	m_bShadow = pDesc->bShadow;
	m_isDecal = pDesc->isDecal;
	m_isARM = pDesc->isARM;
	m_isNormal = pDesc->isNormal;
	m_bUseWorldColor = pDesc->bUseWorldColor;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	memcpy(&m_tDesc, pDesc, sizeof(NONMODEL_DESC));

	m_iStaticHashId++;
	m_iHashId = m_iStaticHashId;

	return S_OK;
}

void CNonAnimModel::Priority_Update(_float fTimeDelta)
{
}

void CNonAnimModel::Update(_float fTimeDelta)
{
	/*if ((GetKeyState(VK_LBUTTON) & 0x8000) &&
		true == m_pVIBufferCom->isPicking(m_pTransformCom->Get_WorldMatrix(), &vPickPos))
	{
		if (CImGui_Manager::Get_Instance()->Get_IsPicking())
		{
			CImGui_Manager::Get_Instance()->Set_ModelPos(vPickPos);
		}
	}*/
}

void CNonAnimModel::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	if(m_isDecal)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_DECAL, this);
	else
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	
	}
	m_pGameInstance->Add_RenderObject(CRenderer::RG_PICKING, this);
}

HRESULT CNonAnimModel::Render()
{
	//if (m_isInstance)
	//{
	//	m_pModelCom->Add_InstanceData(m_pTransformCom->Get_WorldMatrix());
	//	return S_OK;
	//}

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (m_isDecal)
	{
		if(FAILED(Render_Decal()))
			return E_FAIL;
	}
	else
	{
		if (FAILED(Render_NonAnim()))
			return E_FAIL;
	}
	
}

HRESULT CNonAnimModel::Render_Picking()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrix_Ptr())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	uint32_t hash = static_cast<uint32_t>(m_iHashId * 100); // 임의의 상수로 인덱스를 해시처럼 변환

	UINT8 a = (hash >> 24) & 0xff;
	UINT8 r = (hash >> 16) & 0xff;
	UINT8 g = (hash >> 8) & 0xff;
	UINT8 b = (hash) & 0xff;

	_float4 fColor = _float4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHashColor", &fColor, sizeof(_float4))))
		return E_FAIL;

	if (m_isDecal)
	{
		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		m_pVIBufferCom->Bind_Buffers();

		m_pVIBufferCom->Render();
	}
	else
	{
		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{

			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render((_uint)i)))
				return E_FAIL;
		}
	}
	
	return S_OK;
}

_bool CNonAnimModel::Is_Pick(_float3* vPickPos)
{
	if (nullptr == m_pModelCom)
		return false;

	return false;
}

HRESULT CNonAnimModel::Ready_Components(NONMODEL_DESC* pNonAnimDesc)
{
	if (m_isDecal)
	{
		/* FOR.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_SSD"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pNonAnimDesc->szTextureTag_Diffuse, MAX_PATH, m_szTextureTag_Diffuse, MAX_PATH);
		if (FAILED(__super::Add_Component(LEVEL_TOOL, m_szTextureTag_Diffuse,
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom_Diffuse))))
			return E_FAIL;

		if(m_isNormal)
		{
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pNonAnimDesc->szTextureTag_Normal, MAX_PATH, m_szTextureTag_Normal, MAX_PATH);
			if (FAILED(__super::Add_Component(LEVEL_TOOL, m_szTextureTag_Normal,
				TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pTextureCom_Normal))))
				return E_FAIL;
		}
		if (m_isARM)
		{
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pNonAnimDesc->szTextureTag_ARM, MAX_PATH, m_szTextureTag_ARM, MAX_PATH);
			if (FAILED(__super::Add_Component(LEVEL_TOOL, m_szTextureTag_ARM,
				TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom_ARM))))
				return E_FAIL;
		}
		/* FOR.Com_VIBuffer */
		if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Cube"),
			TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
			return E_FAIL;
	}
	else
	{
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pNonAnimDesc->szModelTag, MAX_PATH, m_szModelTag, MAX_PATH);

		/* FOR.Com_Shader */
		if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Model"),
			TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
			return E_FAIL;

		if (m_isInstance)
		{
			CModel* pInstanceModel = m_pGameInstance->Add_NonAnimModel_Instance(LEVEL_TOOL, m_szModelTag);
			if (nullptr == pInstanceModel)
				return E_FAIL;

			if (FAILED(__super::Add_Component(TEXT("Com_Model"), pInstanceModel)))
				return E_FAIL;

			m_pModelCom = pInstanceModel;
			Safe_AddRef(m_pModelCom);
		}
		else
		{
			/* FOR.Com_Model */
			if (FAILED(__super::Add_Component(LEVEL_TOOL, m_szModelTag,
				TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
				return E_FAIL;
		}
	}
	

	return S_OK;
}

HRESULT CNonAnimModel::Render_Decal()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_vDecalWorldInverse", &m_pTransformCom->Get_WorldMatrix_Inverse())))
		return E_FAIL;

	if (FAILED(m_pTextureCom_Diffuse->Bind_ShadeResource(m_pShaderCom, "g_DeacalDiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("bNormal", &m_isNormal, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("bARM", &m_isARM, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("bUseWorldColor", &m_bUseWorldColor, sizeof(_bool))))
		return E_FAIL;

	if (m_isNormal)
	{
		if (FAILED(m_pTextureCom_Normal->Bind_ShadeResource(m_pShaderCom, "g_DeacalNormalTexture", 0)))
			return E_FAIL;
	}
	if (m_isARM)
	{
		if (FAILED(m_pTextureCom_ARM->Bind_ShadeResource(m_pShaderCom, "g_DeacalARMTexture", 0)))
			return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_ARM"), "g_ARMTexture")))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();
	
	return S_OK;
}

HRESULT CNonAnimModel::Render_NonAnim()
{
	_bool bFalse = false;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bSelect", &bFalse, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isLight", &bFalse, sizeof(_bool))))
		return E_FAIL;

	if (m_isInstance)
	{
		m_pModelCom->Add_InstanceData(m_pTransformCom->Get_WorldMatrix());
		return S_OK;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_bSelect", &m_bSelected, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isLight", &m_isLight, sizeof(_bool))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::ROUGHNESS))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ARMTexture", ROUGHNESS, (_uint)i)))
				return E_FAIL;
		}
		if (m_isLight == false)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
				return E_FAIL;
		}

		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::NORMALS))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", NORMALS, (_uint)i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(1)))
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

CNonAnimModel* CNonAnimModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNonAnimModel* pInstance = new CNonAnimModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CNonAnimModel::Clone(void* pArg)
{
	CNonAnimModel* pInstance = new CNonAnimModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNonAnimModel::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom_Diffuse);
	Safe_Release(m_pTextureCom_Normal);
	Safe_Release(m_pTextureCom_ARM);
	Safe_Release(m_pVIBufferCom);
}
