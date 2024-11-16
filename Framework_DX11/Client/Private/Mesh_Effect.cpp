#include "stdafx.h"
#include "Mesh_Effect.h"
#include "GameInstance.h"

CMesh_Effect::CMesh_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Base(pDevice, pContext)
{
}

CMesh_Effect::CMesh_Effect(const CMesh_Effect& Prototype)
	: CEffect_Base(Prototype)
{
}

HRESULT CMesh_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMesh_Effect::Initialize(void* pArg)
{
	MESH_EFFECT_DESC* pDesc = static_cast<MESH_EFFECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc->TextDesc)))
		return E_FAIL;

	m_DefaultDesc = pDesc->DefaultDesc;
	m_InitDesc = m_DefaultDesc;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_DefaultDesc.vPos);
	m_pTransformCom->Set_Scaled(m_DefaultDesc.vStartScale.x, m_DefaultDesc.vStartScale.y, m_DefaultDesc.vStartScale.z);
	m_pTransformCom->Rotation(m_DefaultDesc.vStartRotation.x, m_DefaultDesc.vStartRotation.y, m_DefaultDesc.vStartRotation.z);

	m_eEffectType = TYPE_MESH;


	return S_OK;
}

void CMesh_Effect::Priority_Update(_float fTimeDelta)
{
	if (true == m_isDead)
		return;

}

void CMesh_Effect::Update(_float fTimeDelta)
{
	if (true == m_isDead)
		return;

	m_DefaultDesc.fAlpha += fTimeDelta * m_DefaultDesc.fAlphaSpeed;

	m_pTransformCom->Turn(m_DefaultDesc.vRotationAxis, fTimeDelta * m_DefaultDesc.fRotationSpeed);

	_Vec3 vScale = m_pTransformCom->Get_Scaled();
	vScale += m_DefaultDesc.vScalingSpeed * fTimeDelta;
	m_pTransformCom->Set_Scaled(vScale.x, vScale.y, vScale.z);

	m_vCurrentTileMove += m_DefaultDesc.vTileMoveDir * m_DefaultDesc.fTileMoveSpeed * fTimeDelta;

	__super::Set_WorldMatrix();
}

void CMesh_Effect::Late_Update(_float fTimeDelta)
{
	if (true == m_isDead)
		return;

	m_fAccumulateTime += fTimeDelta;

	if (m_DefaultDesc.fDuration < m_fAccumulateTime)
		m_isActive = false;

	if (CRenderer::RG_EFFECT == m_RenderDesc.iRenderGroup)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this);
	else
		m_pGameInstance->Add_RenderObject((CRenderer::RENDERGROUP)m_RenderDesc.iRenderGroup, this);
}

HRESULT CMesh_Effect::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_DefaultDesc.vColor, sizeof m_DefaultDesc.vColor)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTileRepeat", &m_DefaultDesc.vTileRepeat, sizeof m_DefaultDesc.vTileRepeat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTileMove", &m_vCurrentTileMove, sizeof m_vCurrentTileMove)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_DefaultDesc.fAlpha, sizeof m_DefaultDesc.fAlpha)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(m_DefaultDesc.iShaderIndex)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CMesh_Effect::Reset()
{
	m_DefaultDesc = m_InitDesc;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_DefaultDesc.vPos);
	m_pTransformCom->Rotation(m_DefaultDesc.vStartRotation.x, m_DefaultDesc.vStartRotation.y, m_DefaultDesc.vStartRotation.z);
	m_pTransformCom->Set_Scaled(m_DefaultDesc.vStartScale.x, m_DefaultDesc.vStartScale.y, m_DefaultDesc.vStartScale.z);
}

HRESULT CMesh_Effect::Ready_Components(const TEXT_DESC& Desc)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Effect_Mesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.szModelTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (0 != wcscmp(NONE_TEXT, Desc.szDiffuseTexturTag))
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.szDiffuseTexturTag, TEXT("Com_DiffuseTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_DIFFUSE]))))
			return E_FAIL;
	}

	if (0 != wcscmp(NONE_TEXT, Desc.szNormalTextureTag))
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.szNormalTextureTag, TEXT("Com_NormalTexture"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_NORMAL]))))
			return E_FAIL;
	}

	if (0 != wcscmp(NONE_TEXT, Desc.szMaskTextureTag_1))
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.szMaskTextureTag_1, TEXT("Com_MaskTexture_1"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK_1]))))
			return E_FAIL;
	}

	if (0 != wcscmp(NONE_TEXT, Desc.szMaskTextureTag_2))
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Desc.szMaskTextureTag_2, TEXT("Com_MaskTexture_2"), reinterpret_cast<CComponent**>(&m_pTextureCom[TEXTURE_MASK_2]))))
			return E_FAIL;
	}

	return S_OK;
}

CMesh_Effect* CMesh_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMesh_Effect* pInstance = new CMesh_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Create Failed : CMesh_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMesh_Effect::Clone(void* pArg)
{
	CMesh_Effect* pInstance = new CMesh_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Clone Failed : CMesh_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	for (size_t i = 0; i < TEXTURE_END; ++i)
		Safe_Release(m_pTextureCom[i]);
}
