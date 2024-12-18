#include "stdafx.h"
#include "Raxasia_Sword_CutScene.h"

#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect_Container.h"

CRaxasia_Sword_CutScene::CRaxasia_Sword_CutScene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CRaxasia_Sword_CutScene::CRaxasia_Sword_CutScene(const CRaxasia_Sword_CutScene& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CRaxasia_Sword_CutScene::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRaxasia_Sword_CutScene::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
	m_pParentMatrix = pDesc->pParentWorldMatrix;

	CGameObject::GAMEOBJECT_DESC		Desc{};
	Desc.fSpeedPerSec = 1.5f;
	Desc.fRotationPerSec = 30.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	_uint iAnimIndex = m_pModelCom->Find_AnimationIndex("AS_WP_MOB_Raxasia_01_Sword_Cine_Change__Anim", 1.f);
	m_pModelCom->SetUp_NextAnimation(iAnimIndex, false, 0.f, 1);

	m_pModelCom->Play_Animation(1.f);
	m_isPlayAnimation = false;

	/*	static _float fX = 243.f;
	static _float fY = -5.f;
	static _float fZ = 30.f;*/

	m_isUpdatePos = true;

	return S_OK;
}

void CRaxasia_Sword_CutScene::Priority_Update(_float fTimeDelta)
{

}

void CRaxasia_Sword_CutScene::Update(_float fTimeDelta)
{
	if (m_isPlayAnimation)
		m_pModelCom->Play_Animation(fTimeDelta);
	else
		m_pModelCom->Update_Bone();

	//static _float fXPos = 0.03f;
	//static _float fYPos = -0.03f;
	//static _float fZPos = 0.f;

	//static _float fXRot = 240.f;
	//static _float fYRot = -6.5f;
	//static _float fZRot = 31.f;

	//_matrix		PreTransformMatrix = XMMatrixIdentity();

	//if (KEY_HOLD(KEY::CTRL))
	//{
	//	if (KEY_TAP(KEY::NUM1))
	//	{
	//		fXRot -= 1.f;
	//	}
	//	else if (KEY_TAP(KEY::NUM2))
	//	{
	//		fXRot += 1.f;
	//	}

	//	if (KEY_TAP(KEY::NUM3))
	//	{
	//		fYRot -= 1.f;
	//	}
	//	else if (KEY_TAP(KEY::NUM4))
	//	{
	//		fYRot += 1.f;
	//	}

	//	if (KEY_TAP(KEY::NUM5))
	//	{
	//		fZRot -= 1.f;
	//	}
	//	else if (KEY_TAP(KEY::NUM6))
	//	{
	//		fZRot += 1.f;
	//	}
	//}
	//else
	//{
	//	if (KEY_TAP(KEY::NUM1))
	//	{
	//		fXPos -= 0.01f;
	//	}
	//	else if (KEY_TAP(KEY::NUM2))
	//	{
	//		fXPos += 0.01f;
	//	}

	//	if (KEY_TAP(KEY::NUM3))
	//	{
	//		fYPos -= 0.01f;
	//	}
	//	else if (KEY_TAP(KEY::NUM4))
	//	{
	//		fYPos += 0.01f;
	//	}
	//	if (KEY_TAP(KEY::NUM5))
	//	{
	//		fZPos -= 0.01f;
	//	}
	//	else if (KEY_TAP(KEY::NUM6))
	//	{
	//		fZPos += 0.01f;
	//	}
	//}

	//PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(fXRot)) * XMMatrixRotationY(XMConvertToRadians(fYRot)) * XMMatrixRotationZ(XMConvertToRadians(fZRot));

	//m_pModelCom->Set_PreTranformMatrix(PreTransformMatrix);

	//_Vec3 vv = _Vec3(fXPos, fYPos, fZPos);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vv);


	static _Matrix WorldMatrix;

	if (m_isUpdatePos)
	{
		_matrix		SocketMatrix = *m_pSocketMatrix;

		for (size_t i = 0; i < 3; i++)
		{
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
		}
		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

		WorldMatrix = m_WorldMatrix;
	}
}

void CRaxasia_Sword_CutScene::Late_Update(_float fTimeDelta)
{

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CRaxasia_Sword_CutScene::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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

		m_fEmissiveMask = 0.f;
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

void CRaxasia_Sword_CutScene::Play_Animation(_char* szAnimationName, _float fSpeedRation)
{
	_uint iAnimationIndex = m_pModelCom->Find_AnimationIndex(szAnimationName, fSpeedRation);

	m_pModelCom->SetUp_NextAnimation(iAnimationIndex, false, 0.f, 0, true, true);

	m_isPlayAnimation = true;
}

void CRaxasia_Sword_CutScene::Stop_Animation()
{
	m_isPlayAnimation = false;
}

void CRaxasia_Sword_CutScene::Change_SocketMatrix(const _Matrix* pSocketMatrix)
{
	m_pSocketMatrix = pSocketMatrix;
}

void CRaxasia_Sword_CutScene::Start_UpdatePos()
{
	m_isUpdatePos = true;
}

void CRaxasia_Sword_CutScene::Stop_UpdatePos()
{
	m_isUpdatePos = false;
}

HRESULT CRaxasia_Sword_CutScene::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Raxasia_Sword_CutScene"),
		TEXT("Com_P1Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

CRaxasia_Sword_CutScene* CRaxasia_Sword_CutScene::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRaxasia_Sword_CutScene* pInstance = new CRaxasia_Sword_CutScene(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CRaxasia_Sword_CutScene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CRaxasia_Sword_CutScene::Clone(void* pArg)
{
	CRaxasia_Sword_CutScene* pInstance = new CRaxasia_Sword_CutScene(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRaxasia_Sword_CutScene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRaxasia_Sword_CutScene::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}