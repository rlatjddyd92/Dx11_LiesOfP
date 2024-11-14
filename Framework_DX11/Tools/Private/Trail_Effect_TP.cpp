#include "stdafx.h"
#include "Trail_Effect_TP.h"
#include "GameInstance.h"

CTrail_Effect_TP::CTrail_Effect_TP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base(pDevice, pContext)
{
}

CTrail_Effect_TP::CTrail_Effect_TP(const CTrail_Effect_TP& Prototype)
	: CEffect_Base(Prototype)
{
}

HRESULT CTrail_Effect_TP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrail_Effect_TP::Initialize(void* pArg)
{
	TRAIL_TP_DESC* pDesc = static_cast<TRAIL_TP_DESC*>(pArg);
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	m_ActionDesc = pDesc->InitDesc.ActionDesc;
	m_SaveDesc = *pDesc;

	m_vTestTop = _float3(5.f, 0.f, 0.f);
	m_vTestBottom = _float3(1.f, 0.f, 0.f);

	return S_OK;
}

void CTrail_Effect_TP::Priority_Update(_float fTimeDelta)
{
}

void CTrail_Effect_TP::Update(_float fTimeDelta)
{
	_vector vTopDir = XMLoadFloat3(&m_vTestTop) - XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), 15.f * fTimeDelta);
	vTopDir = XMVector3TransformNormal(vTopDir, RotationMatrix);

	XMStoreFloat3(&m_vTestTop, vTopDir);

	_vector vBottomDir = XMLoadFloat3(&m_vTestBottom) - XMVectorSet(0.f, 0.f, 0.f, 1.f);
	RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), 15.f * fTimeDelta);
	vBottomDir = XMVector3TransformNormal(vBottomDir, RotationMatrix);

	XMStoreFloat3(&m_vTestBottom, vBottomDir);

	m_pVIBufferCom->Update_Buffer(XMLoadFloat3(&m_vTestTop), XMLoadFloat3(&m_vTestBottom), fTimeDelta);
}

void CTrail_Effect_TP::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_DISTORTION, this);
}

HRESULT CTrail_Effect_TP::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pDiffuseTextureCom->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pMaskTextureCom_1->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture_1", 0)))
		return E_FAIL;
	if (FAILED(m_pMaskTextureCom_2->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture_2", 0)))
		return E_FAIL;

	_int iNumInstance = { 15 };
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumInstance", &iNumInstance, sizeof(iNumInstance))))
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Begin(m_ActionDesc.iShaderIndex)))
		return E_FAIL;*/
	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;

	return S_OK;
}

void CTrail_Effect_TP::Reset()
{
	m_pVIBufferCom->Reset();
}

HRESULT CTrail_Effect_TP::Save(_wstring strFilePath)
{
	return S_OK;
}

void CTrail_Effect_TP::Set_Desc(const ACTION_DESC& desc)
{
	m_ActionDesc = desc;
	m_SaveDesc.InitDesc.ActionDesc = desc;
}

CTrail_Effect_TP::TRAIL_TP_DESC CTrail_Effect_TP::Get_Desc()
{
	TRAIL_TP_DESC desc = {};
	desc.BufferDesc = m_SaveDesc.BufferDesc;
	desc.InitDesc.ActionDesc = m_ActionDesc;
	return desc;

}

HRESULT CTrail_Effect_TP::Ready_Components(const TRAIL_TP_DESC& Desc)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Trail_TwoPoint_Instance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	Desc.BufferDesc.iNumInstance;
	Desc.BufferDesc.vLifeTime;

	CVIBuffer_Instancing::INSTANCE_DESC InstDesc = {};

	//InstDesc.iNumInstance = Desc.BufferDesc.iNumInstance;
	//InstDesc.vLifeTime = Desc.BufferDesc.vLifeTime;
	InstDesc.iNumInstance = 15;
	InstDesc.vLifeTime = _float2(1.f, 1.f);

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Trail_TwoPoint_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &InstDesc)))
		return E_FAIL;
	

	//Desc.InitDesc.TextureDesc.strDiffuseTextureTag;
	// Prototype_Component_Texture_Thunder
	// Prototype_Component_Texture_Test_Trail
	// Prototype_Component_Texture_Tile_Spark
	/* FOR.Com_DiffuseTexture */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Test_Trail"),
		TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom))))
		return E_FAIL;
	
	/* FOR.Com_MaskTexture_1 */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_DefaultBlack"),
		TEXT("Com_MaskTexture_1"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom_1))))
		return E_FAIL;

	/* FOR.Com_MaskTexture_2 */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Texture_DefaultBlack"),
		TEXT("Com_MaskTexture_2"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom_2))))
		return E_FAIL;

	return S_OK;
}

CTrail_Effect_TP* CTrail_Effect_TP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrail_Effect_TP* pInstance = new CTrail_Effect_TP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CTrail_Effect_TP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrail_Effect_TP::Clone(void* pArg)
{
	CTrail_Effect_TP* pInstance = new CTrail_Effect_TP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CTrail_Effect_TP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_Effect_TP::Free()
{
	__super::Free();

	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pMaskTextureCom_1);
	Safe_Release(m_pMaskTextureCom_2);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
