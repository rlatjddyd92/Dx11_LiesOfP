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

	if (FAILED(Ready_Components(pDesc->PrototypeDesc)))
		return E_FAIL;

	m_ActionDesc = pDesc->ActionDesc;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_ActionDesc.vPos);
	m_pTransformCom->Set_Scaled(m_ActionDesc.vScale.x, m_ActionDesc.vScale.y, m_ActionDesc.vScale.z);
	m_pTransformCom->Rotation(m_ActionDesc.vStartRotation.x, m_ActionDesc.vStartRotation.y, m_ActionDesc.vStartRotation.z);

	m_SaveDesc = *pDesc;
	
	return S_OK;
}

void CMesh_Effect::Priority_Update(_float fTimeDelta)
{
	
}

void CMesh_Effect::Update(_float fTimeDelta)
{
	m_ActionDesc.fAlpha += fTimeDelta * m_ActionDesc.fAlphaSpeed;

	m_pTransformCom->Turn(m_ActionDesc.vRotationAxis, fTimeDelta * m_ActionDesc.fRotationSpeed);

	_Vec3 vScale = m_pTransformCom->Get_Scaled();
	vScale += m_ActionDesc.vScalingSpeed * fTimeDelta;
	m_pTransformCom->Set_Scaled(vScale.x, vScale.y, vScale.z);
	
	m_vCurrentTileMove += m_ActionDesc.vTileMoveDir * m_ActionDesc.fTileMoveSpeed * fTimeDelta;

	__super::Set_WorldMatrix();

}

void CMesh_Effect::Late_Update(_float fTimeDelta)
{
	m_fAccumulateTime += fTimeDelta;

	if (m_ActionDesc.fDuration < m_fAccumulateTime)
		m_isActive = false;

	
	if (RS_BLEND & m_ActionDesc.iState)
	{
		if (FAILED(m_pGameInstance->Add_RenderObject(CRenderer::RG_BLEND, this)))
			return;
	}
	if (RS_NONBLEND & m_ActionDesc.iState)
	{
		if (FAILED(m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this)))
			return;
	}
	if (RS_NONLIGHT & m_ActionDesc.iState)
	{
		if (FAILED(m_pGameInstance->Add_RenderObject(CRenderer::RG_NONLIGHT, this)))
			return;
	}
	if (RS_DISTORTION & m_ActionDesc.iState)
	{
		if (FAILED(m_pGameInstance->Add_RenderObject(CRenderer::RG_DISTORTION, this)))
			return;
	}
	if (RS_BLUR & m_ActionDesc.iState)
	{

	}
}

HRESULT CMesh_Effect::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_ActionDesc.vColor, sizeof m_ActionDesc.vColor)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTileRepeat", &m_ActionDesc.vTileRepeat, sizeof m_ActionDesc.vTileRepeat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vTileMove", &m_vCurrentTileMove, sizeof m_vCurrentTileMove)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_ActionDesc.fAlpha, sizeof m_ActionDesc.fAlpha)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(m_ActionDesc.iShaderIndex)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CMesh_Effect::Set_Desc(const ACTION_DESC& desc)
{
	m_ActionDesc = desc;
}

CMesh_Effect::ACTION_DESC CMesh_Effect::Get_Desc()
{
	return m_ActionDesc;
}

void CMesh_Effect::Reset()
{
}

HRESULT CMesh_Effect::Save(_wstring strFilePath)
{
	return S_OK;
}

HRESULT CMesh_Effect::Ready_Components(const PROTOTYPE_DESC& desc)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Effect_Mesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, desc.szModelTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Diffuse */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, desc.strDiffuseTexturTag, TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom))))
		return E_FAIL;

	/* FOR.Com_Mask_1 */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, desc.strMaskTextureTag_1, TEXT("Com_Mask_1"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom_1))))
		return E_FAIL;

	/* FOR.Com_Mask_2 */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, desc.strMaskTextureTag_2, TEXT("Com_Mask_2"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom_2))))
		return E_FAIL;

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
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pMaskTextureCom_1);
	Safe_Release(m_pMaskTextureCom_2);
}
