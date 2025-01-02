#include "stdafx.h"
#include "Butterfly.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect_Container.h"

CButterfly::CButterfly(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}


CButterfly::CButterfly(const CButterfly& Prototype)
    : CGameObject{ Prototype }
{
}

HRESULT CButterfly::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CButterfly::Initialize(void* pArg)
{
	OBJECT_DEFAULT_DESC* pDesc = static_cast<OBJECT_DEFAULT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	//m_iAnimIndex = m_pModelCom->Find_AnimationIndex("AS_Idle_C", 1.f);	//가만히 날개짓
	//m_iAnimIndex = m_pModelCom->Find_AnimationIndex("AS_Walk_C_B_Root", 1.f);	//뒤로 이동
	m_iAnimIndex = m_pModelCom->Find_AnimationIndex("AS_Walk_C_F", 1.f);	 //앞으로 이동	
	//m_iAnimIndex = m_pModelCom->Find_AnimationIndex("AS_Walk_C_F_Root", 1.f); //앞으로 이동(루트 이동)
	//m_iAnimIndex = m_pModelCom->Find_AnimationIndex("AS_Walk_C_L_Root", 1.f);	//왼쪽으로 이동(루트 이동)
	//m_iAnimIndex = m_pModelCom->Find_AnimationIndex("AS_Walk_C_R_Root", 1.f);	//오른쪽으로 이동(루트 이동)

	m_pModelCom->SetUp_Animation(m_iAnimIndex, true);
	m_pTransformCom->Set_Scaled(0.15f, 0.15f, 0.15f);
	m_pTransformCom->Rotation(0.f, 155.f, 0.f);

	m_Effect = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("ButterFly_Fly"), m_pTransformCom->Get_WorldMatrix_Ptr(), nullptr, _Vec3(0.f, 0.f, 0.0f));
	m_Effect->Set_Loop(true);

	return S_OK;
}

void CButterfly::Priority_Update(_float fTimeDelta)
{
	if (m_bFirstPosSetting == false)
	{
		m_bFirstPosSetting = true;
		CGameObject* pSophia = m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_Sophia"), 0);
		_Vec3 vNewPos = pSophia->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		m_vNewPos = vNewPos;
		m_vNewPos.x = m_vNewPos.x + 0.65f + m_vPosOffset.x;
		m_vNewPos.y = m_vNewPos.y + 1.67f + m_vPosOffset.y;
		m_vNewPos.z = m_vNewPos.z - 0.2f + m_vPosOffset.z;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vNewPos);
	}
	m_Effect->Priority_Update(fTimeDelta);
}

void CButterfly::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
	m_Effect->Update(fTimeDelta);
	m_pTransformCom->Go_Right(fTimeDelta, 0.3f);

	//static _float fX = m_vNewPos.x;
	//static _float fY = m_vNewPos.y;
	//static _float fZ = m_vNewPos.z;

	//if (KEY_TAP(KEY::NUM1))
	//{
	//	fX -= 0.05f;
	//}
	//else if (KEY_TAP(KEY::NUM4))
	//{
	//	fX += 0.05f;
	//}

	//if (KEY_TAP(KEY::NUM2))
	//{
	//	fY -= 0.05f;
	//}
	//else if (KEY_TAP(KEY::NUM5))
	//{
	//	fY += 0.05f;
	//}

	//if (KEY_TAP(KEY::NUM3))
	//{
	//	fZ -= 0.05f;
	//}
	//else if (KEY_TAP(KEY::NUM6))
	//{
	//	fZ += 0.05f;
	//}

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, _Vec3(fX, fY, fZ));

}

void CButterfly::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_Effect->Late_Update(fTimeDelta);
}

HRESULT CButterfly::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", DIFFUSE, (_uint)i)))
			return E_FAIL;

		// EMISSIVE
		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::EMISSIVE))
		{
			m_fEmissiveMask = 1.f;
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmessiveTexture", EMISSIVE, (_uint)i)))
				return E_FAIL;
		}
		else
		{
			m_fEmissiveMask = 0.f;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmessiveMask", &m_fEmissiveMask, sizeof(_float))))
			return E_FAIL;

		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::ROUGHNESS))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ARMTexture", ROUGHNESS, (_uint)i)))
				return E_FAIL;
		}

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

	m_fEmissiveMask = 0.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmessiveMask", &m_fEmissiveMask, sizeof(_float))))
	return E_FAIL;

	return S_OK;
}

HRESULT CButterfly::Ready_Components(OBJECT_DEFAULT_DESC* pDesc)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Butterfly"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

    return S_OK;
}

CButterfly* CButterfly::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CButterfly* pInstance = new CButterfly(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CButterfly"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CButterfly::Clone(void* pArg)
{
	CButterfly* pInstance = new CButterfly(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CButterfly"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CButterfly::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		m_Effect->Set_Cloned(false);
		Safe_Release(m_Effect);
	}
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
