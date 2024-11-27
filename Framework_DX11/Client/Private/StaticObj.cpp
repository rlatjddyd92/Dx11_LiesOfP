#include "stdafx.h"
#include "StaticObj.h"
#include "GameInstance.h"

CStaticObj::CStaticObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}


CStaticObj::CStaticObj(const CStaticObj& Prototype)
	: CGameObject{ Prototype }
{
}


HRESULT CStaticObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStaticObj::Initialize(void* pArg)
{
	STATICOBJ_DESC* pDesc = static_cast<STATICOBJ_DESC*>(pArg);

	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pDesc->vPosition));
	m_isInstance = pDesc->isInstance;
	m_bShadow = pDesc->bShadow;
	m_iCurrentCellNum = pDesc->iCurrentCellNum;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CStaticObj::Priority_Update(_float fTimeDelta)
{
}

void CStaticObj::Update(_float fTimeDelta)
{
}

void CStaticObj::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fCullDistance))
	{
		/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
		__super::Late_Update(fTimeDelta);

		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);	
		//if(m_bShadow)

	}
	//m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);
}

HRESULT CStaticObj::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (m_isInstance)
	{
		INSTANCE_DATA tData{};
		tData.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		tData.vHashColor = m_vHashColor;

		m_pModelCom->Add_InstanceData(tData);
		return S_OK;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{

		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::ROUGHNESS))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ARMTexture", ROUGHNESS, (_uint)i)))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;
	
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

HRESULT CStaticObj::Render_LightDepth()
{
	if (m_isInstance)
	{
		INSTANCE_DATA tData{};
		tData.WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		tData.vHashColor = m_vHashColor;

		m_pModelCom->Add_InstanceData(tData);
		return S_OK;
	}

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4		ViewMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 20.f, -15.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeViewMatrix", m_pGameInstance->Get_CascadeViewMatirx(), 3)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeProjMatrix", m_pGameInstance->Get_CascadeProjMatirx(), 3)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CStaticObj::Ready_Components(STATICOBJ_DESC* pNonAnimDesc)
{
	_tchar szModelTag[MAX_PATH];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pNonAnimDesc->szModelTag, MAX_PATH, szModelTag, MAX_PATH);

	////유독 큰 물체 컬링 예외처리
	//if (wcscmp(szModelTag, TEXT("SM_Monastery_Floor_06")) == 0)
	//	m_fCullDistance = 150.f;

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (m_isInstance)
	{
		CModel* pInstanceModel = m_pGameInstance->Add_NonAnimModel_Instance(LEVEL_GAMEPLAY, szModelTag);
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
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, szModelTag,
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;
	}
	return S_OK;
}

CStaticObj* CStaticObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStaticObj* pInstance = new CStaticObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CStaticObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CStaticObj::Clone(void* pArg)
{
	CStaticObj* pInstance = new CStaticObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CStaticObj"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStaticObj::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
