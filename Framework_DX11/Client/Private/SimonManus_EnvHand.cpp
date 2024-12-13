#include "stdafx.h"
#include "SimonManus_EnvHand.h"

#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect_Container.h"

CSimonManus_EnvHand::CSimonManus_EnvHand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CSimonManus_EnvHand::CSimonManus_EnvHand(const CSimonManus_EnvHand& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CSimonManus_EnvHand::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSimonManus_EnvHand::Initialize(void* pArg)
{
	_uint* iEnvHandAnimIndex = static_cast<_uint*>(pArg);

	CGameObject::GAMEOBJECT_DESC		Desc{};
	Desc.fSpeedPerSec = 1.5f;
	Desc.fRotationPerSec = 30.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(10.f, 10.f, 10.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _Vec3(0.3f, 17.45f, 0.7f));
	m_pTransformCom->Rotation(_vector{ 0, 1, 0, 0 }, XMConvertToRadians(50.f));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	string strLowerAnimName = "AS_Env_hand_pose_0" + to_string(*iEnvHandAnimIndex);

	_uint iLowerAnimationIndex = m_pModelCom->Find_AnimationIndex(strLowerAnimName.c_str(), 1.f);
	_uint iUpperAnimationIndex = m_pModelCom->Find_AnimationIndex("AS_Env_hand_finger_Loop_add", 1.f);

	m_pModelCom->SetUp_NextAnimation(iUpperAnimationIndex, true, 0.f, 0, false);
	m_pModelCom->SetUp_NextAnimation_Boundary(iLowerAnimationIndex, true, 0.f, 0, false);

	m_pModelCom->Play_Animation(1.f);
	
	return S_OK;
}

void CSimonManus_EnvHand::Priority_Update(_float fTimeDelta)
{

	
}

void CSimonManus_EnvHand::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

}

void CSimonManus_EnvHand::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CSimonManus_EnvHand::Render()
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

		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::ROUGHNESS))
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ARMTexture", ROUGHNESS, (_uint)i)))
				return E_FAIL;
		}

		// EMISSIVE
		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::EMISSIVE))
		{
			m_fEmissiveMask = 0.f;
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmessiveTexture", EMISSIVE, (_uint)i)))
				return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmessiveMask", &m_fEmissiveMask, sizeof(_float))))
			return E_FAIL;

		//RimLight
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimLight", &m_vRimLightColor, sizeof(_float4))))
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

	//RimLight ÃÊ±âÈ­
	_Vec4 vInitRimLight = _Vec4(0.f, 0.f, 0.f, 0.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimLight", &vInitRimLight, sizeof(_float4))))
		return E_FAIL;


	return S_OK;
}

HRESULT CSimonManus_EnvHand::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManus_CutScene_EnvHand"),
		TEXT("Com_P1Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CSimonManus_EnvHand* CSimonManus_EnvHand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSimonManus_EnvHand* pInstance = new CSimonManus_EnvHand(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSimonManus_EnvHand"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSimonManus_EnvHand::Clone(void* pArg)
{
	CSimonManus_EnvHand* pInstance = new CSimonManus_EnvHand(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSimonManus_EnvHand"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSimonManus_EnvHand::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}