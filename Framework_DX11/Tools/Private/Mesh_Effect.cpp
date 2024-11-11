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

	if (FAILED(Ready_Components(*pDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPosition);
	m_pTransformCom->Set_Scaled(pDesc->vScale.x, pDesc->vScale.y, pDesc->vScale.z);
	m_pTransformCom->Rotation(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z);

	return S_OK;
}

void CMesh_Effect::Priority_Update(_float fTimeDelta)
{
}

void CMesh_Effect::Update(_float fTimeDelta)
{
}

void CMesh_Effect::Late_Update(_float fTimeDelta)
{
}

HRESULT CMesh_Effect::Render()
{
	//m_pShaderCom->Bind_RawValue("g_")

	return S_OK;
}

void CMesh_Effect::Reset()
{
}

HRESULT CMesh_Effect::Save(_wstring strFilePath)
{
	return S_OK;
}

HRESULT CMesh_Effect::Ready_Components(const MESH_EFFECT_DESC& desc)
{
	m_pDiffueseTextureCom = { nullptr };
	m_pMaskTextureCom_1 = { nullptr };
	m_pMaskTextureCom_2 = { nullptr };

	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, desc.szModelTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Diffuse */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, desc.strDiffuseTexturTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pDiffueseTextureCom))))
		return E_FAIL;

	/* FOR.Com_Mask_1 */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, desc.strMaskTextureTag_1, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom_1))))
		return E_FAIL;

	/* FOR.Com_Mask_2 */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, desc.strMaskTextureTag_2, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom_2))))
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
}
