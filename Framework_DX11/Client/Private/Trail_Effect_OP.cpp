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

	m_eEffectType = EFFECT_TYPE::TYPE_TRAIL_OP;

	m_DefaultDesc = pDesc->DefaultDesc;
	m_InitDesc = m_DefaultDesc;
		
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_DefaultDesc.vPos);

	return S_OK;
}

void CTrail_Effect_OP::Priority_Update(_float fTimeDelta)
{
	if (false == m_isActive)
		m_isActive = true;
}

void CTrail_Effect_OP::Update(_float fTimeDelta)
{
	if (false == m_isActive)
		return;

	__super::Set_WorldMatrix();

	if(TYPE_UPDATE == m_DefaultDesc.eType)
	{
		if (true == m_pVIBufferCom->Update_Buffer(XMLoadFloat4x4(&m_WorldMatrix).r[3], m_DefaultDesc.fTrailInterval, m_DefaultDesc.bLoop, fTimeDelta))
		{
			m_isDead = true;
			m_isActive = false;
		}
	}
	else if (TYPE_SPREAD == m_DefaultDesc.eType)
	{
		if (true == m_pVIBufferCom->Spread_Buffer(XMLoadFloat4x4(&m_WorldMatrix).r[3], m_DefaultDesc.fTrailInterval, m_DefaultDesc.fSpreadSpeed, m_DefaultDesc.bLoop, fTimeDelta))
		{
			m_isDead = true;
			m_isActive = false;
		}
	}
}

void CTrail_Effect_OP::Late_Update(_float fTimeDelta)
{
	if (false == m_isActive)
		return;

	m_pGameInstance->Add_RenderObject((CRenderer::RENDERGROUP)m_RenderDesc.iRenderGroup, this);
}

HRESULT CTrail_Effect_OP::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Vec4(), sizeof(_Vec4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_DefaultDesc.vColor, sizeof m_DefaultDesc.vColor)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTexDivide", &m_DefaultDesc.vDivide, sizeof m_DefaultDesc.vDivide)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSpriteSpeed", &m_DefaultDesc.fSpriteSpeed, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fScale", &m_DefaultDesc.fScaling, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iState", &m_DefaultDesc.iGeomState, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_DefaultDesc.iShaderIndex)))
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
	m_DefaultDesc = m_InitDesc;
	m_isDead = false;
	m_isActive = false;

}


HRESULT CTrail_Effect_OP::Ready_Components(const TRAIL_OP_DESC& Desc)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail_OnePoint_Instance"),
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

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	for (size_t i = 0; i < TEXTURE_END; ++i)
	{
		Safe_Release(m_pTextureCom[i]);
	}
}
