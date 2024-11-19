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

	m_DefaultDesc = pDesc->DefaultDesc;

	m_InitDesc = m_DefaultDesc;
	m_eEffectType = TYPE_TRAIL_TP;

	m_DefaultDesc.fAlphaSpeed = 1.f;

	return S_OK;
}

void CTrail_Effect_TP::Priority_Update(_float fTimeDelta)
{
}

void CTrail_Effect_TP::Update(_float fTimeDelta)
{
	__super::Set_WorldMatrix();
		
	if(true== m_DefaultDesc.bLoop)
	{
		m_pVIBufferCom->Update_Buffer(
			XMLoadFloat4x4(&m_WorldMatrix).r[3] + XMVector3TransformNormal(m_DefaultDesc.vTopOffset, m_WorldMatrix),
			XMLoadFloat4x4(&m_WorldMatrix).r[3] + XMVector3TransformNormal(m_DefaultDesc.vBottomOffset, m_WorldMatrix),
			fTimeDelta);
	}

	if (false == m_DefaultDesc.bLoop)
		m_fAlpha += fTimeDelta * m_DefaultDesc.fAlphaSpeed;
}

void CTrail_Effect_TP::Late_Update(_float fTimeDelta)
{
	if (false == m_DefaultDesc.bLoop && 1.f < m_fAlpha)
	{
		m_isDead = true;
	}

	if (CRenderer::RG_EFFECT == m_RenderDesc.iRenderGroup)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
	else
		m_pGameInstance->Add_RenderObject((CRenderer::RENDERGROUP)m_RenderDesc.iRenderGroup, this);
}

HRESULT CTrail_Effect_TP::Render()
{
	if (FAILED(__super::Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (nullptr != m_pTextureCom[TEXTURE_DIFFUSE])
	{
		if (FAILED(m_pTextureCom[TEXTURE_DIFFUSE]->Bind_ShadeResource(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}

	if (nullptr != m_pTextureCom[TEXTURE_MASK_1])
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK_1]->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture_1", 0)))
			return E_FAIL;
	}

	if (nullptr != m_pTextureCom[TEXTURE_MASK_2])
	{
		if (FAILED(m_pTextureCom[TEXTURE_MASK_2]->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture_2", 0)))
			return E_FAIL;
	}

	if (nullptr != m_pTextureCom[TEXTURE_NORMAL])
	{
		if (FAILED(m_pTextureCom[TEXTURE_NORMAL]->Bind_ShadeResource(m_pShaderCom, "g_NormalTexture", 0)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumInstance", &m_DefaultDesc.iNumInstance, sizeof(m_DefaultDesc.iNumInstance))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_DefaultDesc.vColor, sizeof(m_DefaultDesc.vColor))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &m_fAlpha, sizeof(m_fAlpha))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_DefaultDesc.iShaderIndex)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CTrail_Effect_TP::Reset()
{
	m_pVIBufferCom->Reset();
	m_DefaultDesc = m_InitDesc;
	m_isDead = false;
	m_fAlpha = 0.f;
}


HRESULT CTrail_Effect_TP::Ready_Components(const TRAIL_TP_DESC& Desc)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail_TwoPoint_Instance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	
	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, Desc.strVIBufferTag,
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	
	if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szDiffuseTexturTag))
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.TextDesc.szDiffuseTexturTag, TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]))))
			return E_FAIL;
	}

	if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szNormalTextureTag))
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.TextDesc.szNormalTextureTag, TEXT("Com_NormalTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NORMAL]))))
			return E_FAIL;
	}

	if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szMaskTextureTag_1))
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.TextDesc.szMaskTextureTag_1, TEXT("Com_MaskTexture_1"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK_1]))))
			return E_FAIL;
	}

	if (0 != wcscmp(NONE_TEXT, Desc.TextDesc.szMaskTextureTag_2))
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.TextDesc.szMaskTextureTag_2, TEXT("Com_MaskTexture_2"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK_2]))))
			return E_FAIL;
	}

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

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	for (size_t i = 0; i < TEXTURE_END; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}

}
