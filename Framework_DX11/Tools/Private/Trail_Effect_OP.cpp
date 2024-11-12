#include "stdafx.h"
#include "Trail_Effect_OP.h"
#include "GameInstance.h"

CTrail_Effect_OP::CTrail_Effect_OP(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base(pDevice, pContext)
{
}

CTrail_Effect_OP::CTrail_Effect_OP(const CTrail_Effect_OP& Prototype)
	: CEffect_Base(Prototype)
{
}

HRESULT CTrail_Effect_OP::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrail_Effect_OP::Initialize(void* pArg)
{
	TRAIL_OP_DESC* pDesc = static_cast<TRAIL_OP_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	m_ActionDesc = pDesc->InitDesc.ActionDesc;
	m_SaveDesc = *pDesc;

	return S_OK;
}

void CTrail_Effect_OP::Priority_Update(_float fTimeDelta)
{
}

void CTrail_Effect_OP::Update(_float fTimeDelta)
{
	_Vec4 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (true == m_bLeft)
		vPos.x += 1.f * fTimeDelta;
	else
		vPos.x -= 1.f * fTimeDelta;

	if (vPos.x > 3.f || vPos.x < -3.f)
		m_bLeft = !m_bLeft;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _Vec3(vPos));

	__super::Set_WorldMatrix();
	m_pVIBufferCom->Update_Buffer(XMLoadFloat4x4(&m_WorldMatrix).r[3], fTimeDelta);
}

void CTrail_Effect_OP::Late_Update(_float fTimeDelta)
{
	if(RS_NONBLEND & m_ActionDesc.iRenderState)
	{
		if (FAILED(m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this)))
			return;
	}
	if (RS_DISTORTION & m_ActionDesc.iRenderState)
	{
		if (FAILED(m_pGameInstance->Add_RenderObject(CRenderer::RG_DISTORTION, this)))
			return;
	}
	if (RS_BLUR & m_ActionDesc.iRenderState)
	{
	}
	if (RS_NONLIGHT & m_ActionDesc.iRenderState)
	{
		if (FAILED(m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this)))
			return;
	}
	if (RS_BLEND & m_ActionDesc.iRenderState)
	{
		if (FAILED(m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this)))
			return;
	}
}

HRESULT CTrail_Effect_OP::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Vec4(), sizeof(_Vec4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_ActionDesc.vColor, sizeof m_ActionDesc.vColor)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexDivide", &m_ActionDesc.vDivide, sizeof m_ActionDesc.vDivide)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteSpeed", &m_ActionDesc.fSpriteSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &m_ActionDesc.fScale, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iState", &m_ActionDesc.iState, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_ActionDesc.iShaderIndex)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CTrail_Effect_OP::Reset()
{
	m_pVIBufferCom->Reset();
}

HRESULT CTrail_Effect_OP::Save(_wstring strFilePath)
{
	if (strFilePath.back() == L'\0')
		strFilePath.resize(strFilePath.size() - 1);

	_wstring strResultPath = strFilePath + TEXT("\\") + m_strEffectName + TEXT(".TE");

	_char FilePath[MAX_PATH] = {};
	int sizeNeeded = WideCharToMultiByte(CP_ACP, 0, strResultPath.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (sizeNeeded > 0 && sizeNeeded <= MAX_PATH)
	{
		WideCharToMultiByte(CP_ACP, 0, strResultPath.c_str(), -1, FilePath, MAX_PATH, nullptr, nullptr);
	}

	ofstream outfile(FilePath, ios::binary);

	if (!outfile.is_open())
		return E_FAIL;
	
	outfile.write(reinterpret_cast<const _char*>(m_SaveDesc.strEffectName), sizeof(m_SaveDesc.strEffectName));
	outfile.write(reinterpret_cast<const _char*>(&m_SaveDesc.BufferDesc), sizeof(m_SaveDesc.BufferDesc));
	outfile.write(reinterpret_cast<const _char*>(&m_SaveDesc.InitDesc), sizeof(m_SaveDesc.InitDesc));

	outfile.close();

	return S_OK;
}

void CTrail_Effect_OP::Set_Desc(const ACTION_DESC& desc)
{
	m_SaveDesc.InitDesc.ActionDesc = desc;
	m_ActionDesc = desc;
}

CTrail_Effect_OP::TRAIL_OP_DESC CTrail_Effect_OP::Get_Desc()
{
	TRAIL_OP_DESC desc = {};
	desc.BufferDesc = m_SaveDesc.BufferDesc;
	desc.InitDesc.ActionDesc = m_ActionDesc;
	return desc;
}

HRESULT CTrail_Effect_OP::Ready_Components(const TRAIL_OP_DESC& Desc)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Trail_OnePoint_Instance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	Desc.BufferDesc.iNumInstance;
	Desc.BufferDesc.vLifeTime;

	CVIBuffer_Instancing::INSTANCE_DESC InstDesc = {};

	InstDesc.iNumInstance = Desc.BufferDesc.iNumInstance;
	InstDesc.vLifeTime = Desc.BufferDesc.vLifeTime;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Trail_OnePoint_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), &InstDesc)))
		return E_FAIL;

	/* FOR.Com_DiffuseTexture */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, Desc.InitDesc.TextureDesc.strDiffuseTextureTag,
		TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom))))
		return E_FAIL;

	/* FOR.Com_MaskTexture_1 */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, Desc.InitDesc.TextureDesc.strMaskTextureTag_1,
		TEXT("Com_MaskTexture_1"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom_1))))
		return E_FAIL;

	/* FOR.Com_MaskTexture_2 */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, Desc.InitDesc.TextureDesc.strMaskTextureTag_2,
		TEXT("Com_MaskTexture_2"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom_2))))
		return E_FAIL;

	return S_OK;
}

CTrail_Effect_OP* CTrail_Effect_OP::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrail_Effect_OP* pInstance = new CTrail_Effect_OP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CTrail_Effect_OP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrail_Effect_OP::Clone(void* pArg)
{
	CTrail_Effect_OP* pInstance = new CTrail_Effect_OP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CTrail_Effect_OP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrail_Effect_OP::Free()
{
	__super::Free();

	Safe_AddRef(m_pShaderCom);
	Safe_AddRef(m_pVIBufferCom);

	Safe_AddRef(m_pDiffuseTextureCom);
	Safe_AddRef(m_pMaskTextureCom_1);
	Safe_AddRef(m_pMaskTextureCom_2);
}
