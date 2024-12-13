#include "stdafx.h"
#include "Scissor_Handle.h"

#include "GameInstance.h"
#include "ImGui_Manager.h"

#include "Scissor_Blade.h"

#include "Controller_EffectTool.h"

CScissor_Handle::CScissor_Handle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CScissor_Handle::CScissor_Handle(const CScissor_Handle& Prototype)
	: CGameObject{ Prototype }
{
}



HRESULT CScissor_Handle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CScissor_Handle::Initialize(void* pArg)
{
	/* 직교투영을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Blade()))
		return E_FAIL;
	//m_pTransformCom->Set_Scaled(10.f, 10.f, 10.f);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _Vec3(0.3f, 17.45f, 0.7f));
	//m_pTransformCom->Rotation(_vector{ 0, 1, 0, 0 }, XMConvertToRadians(50.f));

	m_bRender = true;

	return S_OK;
}

void CScissor_Handle::Priority_Update(_float fTimeDelta)
{
	Change_ScissorType();

	if(m_pBlade != nullptr)
		m_pBlade->Priority_Update(fTimeDelta);
}

void CScissor_Handle::Update(_float fTimeDelta)
{
	if (nullptr != m_pModelCom)
		m_pModelCom->Update_Bone();

	if (m_pBlade != nullptr)
		m_pBlade->Update(fTimeDelta);
}

void CScissor_Handle::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	if (m_pBlade != nullptr)
		m_pBlade->Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CScissor_Handle::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", TEXTURE_TYPE::DIFFUSE, (_uint)i)))
			return E_FAIL;

		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::ROUGHNESS))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ARMTexture", ROUGHNESS, (_uint)i)))
				return E_FAIL;
		}

		// EMISSIVE
		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::EMISSIVE))
		{
			m_fEmissive = 0.3f;
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmessiveTexture", EMISSIVE, (_uint)i)))
				return E_FAIL;
		}
		else
		{
			m_fEmissive = 0.f;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmessiveMask", &m_fEmissive, sizeof(_float))))
			return E_FAIL;

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

	if (m_pBlade != nullptr)
		m_pBlade->Render();

	return S_OK;
}

HRESULT CScissor_Handle::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Scissor_Left_Hnd"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	m_eType = SCISSOR_LEFT;

	return S_OK;
}

HRESULT CScissor_Handle::Ready_Blade()
{
	CScissor_Blade::BLADE_DESC ScissorDesc;
	ScissorDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	ScissorDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Blade");

	if (m_eType == SCISSOR_LEFT)
	{
		ScissorDesc.iWeaponType = SCISSOR_LEFT;
		m_pBlade = dynamic_cast<CScissor_Blade*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ScissorBlade"), &ScissorDesc));
	}
	else if (m_eType == SCISSOR_RIGHT)
	{
		ScissorDesc.iWeaponType = SCISSOR_RIGHT;
		m_pBlade = dynamic_cast<CScissor_Blade*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ScissorBlade"), &ScissorDesc));
		if (nullptr == m_pBlade)
			return E_FAIL;
	}

	if (nullptr == m_pBlade)
		return E_FAIL;

	return S_OK;
}

HRESULT CScissor_Handle::Change_ScissorType()
{
	if (KEY_TAP(KEY::Z))
	{
		if (m_eType == SCISSOR_LEFT)
			return S_OK;

		Safe_Release(m_pModelCom);
		Safe_Release(m_pBlade);

		m_pModelCom = static_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Scissor_Left_Hnd")));

		m_eType = SCISSOR_LEFT;

		if (FAILED(Ready_Blade()))
			return E_FAIL;
	}
	else if (KEY_TAP(KEY::X))
	{
		if (m_eType == SCISSOR_RIGHT)
			return S_OK;

		Safe_Release(m_pModelCom);
		Safe_Release(m_pBlade);

		m_pModelCom = static_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Scissor_Right_Hnd")));

		m_eType = SCISSOR_RIGHT;

		if (FAILED(Ready_Blade()))
			return E_FAIL;
	}

	return S_OK;
}

CScissor_Handle* CScissor_Handle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CScissor_Handle* pInstance = new CScissor_Handle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CScissor_Handle::Clone(void* pArg)
{
	CScissor_Handle* pInstance = new CScissor_Handle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CScissor_Handle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScissor_Handle::Free()
{
	__super::Free();

	Safe_Release(m_pBlade);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
