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

	if (FAILED(Ready_Effect()))
		return E_FAIL;

	m_iAnimation_Phase1Index = m_pModelCom->Find_AnimationIndex("AS_WP_MOB_Raxasia_01_Sword_Cine_Change__Anim", 1.f);
	m_pModelCom->SetUp_NextAnimation(m_iAnimation_Phase1Index, false, 0.f, 1);

	m_iAnimation_Phase2Index = m_pModelCom->Find_AnimationIndex("AS_Sword_Raxasia_Phase2_C06_CINE", 1.f);

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

	m_iFrame = m_pModelCom->Get_CurrentFrame();

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

	_uint iCurAnimIndex = m_pModelCom->Get_CurrentAnimationIndex();
	if (iCurAnimIndex == m_iAnimation_Phase1Index)
	{
		Control_Phase1Effect();
	}
	else if (iCurAnimIndex == m_iAnimation_Phase2Index)
	{
		Control_Phase2Effect();
	}

	if (KEY_TAP(KEY::P))
	{
		_Matrix		WorldMatrix{};
		_matrix		SocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix("BN_Blade10");

		for (size_t i = 0; i < 3; i++)
		{
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
		}
		XMStoreFloat4x4(&WorldMatrix, SocketMatrix * XMLoadFloat4x4(&m_WorldMatrix));

		_Vec3 vCurrentPos = WorldMatrix.Translation();
		_Vec3 vWorldUp = _Vec3(0.f, 1.f, 0.f);

		vCurrentPos -= vWorldUp * 0.04f;

		m_pEffect_Manager->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_CutScene_Weapon_Change_Turn"), vCurrentPos);
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

	//RimLight 초기화
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

void CRaxasia_Sword_CutScene::Control_Phase1Effect()
{
	_float fCurretTrackPosition = m_pModelCom->Get_CurrentTrackPosition();

	_Matrix		WorldMatrix{};
	_matrix		SocketMatrix{};

	_Vec3 vCurrentPos = WorldMatrix.Translation();
	_Vec3 vWorldUp = _Vec3(0.f, 1.f, 0.f);

	// 다 돌았을 때
	for (_uint i = 0; i < 11; ++i)
	{
		if (!m_isActiveTurnEffect[i] && fCurretTrackPosition >= m_iTurnEffectFrame[i])
		{
			_uint iBoneIndex = 11 - i;

			string strBoneName = "BN_Blade";
			string strNum = to_string(iBoneIndex);

			strBoneName += strNum;

			SocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix(strBoneName.c_str());

			for (size_t i = 0; i < 3; i++)
			{
				SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
			}
			XMStoreFloat4x4(&WorldMatrix, SocketMatrix * XMLoadFloat4x4(&m_WorldMatrix));


			vCurrentPos = WorldMatrix.Translation();
			vCurrentPos -= vWorldUp * 0.05f;

			m_pEffect_Manager->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_CutScene_Weapon_Change_Turn"), vCurrentPos);

			m_isActiveTurnEffect[i] = true;
		}
	}

	// 다 돌고 철컹
	for (_uint i = 0; i < 11; ++i)
	{
		if (!m_isActiveInsertEffect[i] && fCurretTrackPosition >= m_iInsertEffectFrame[i])
		{
			_uint iBoneIndex = 11 - i;

			string strBoneName = "BN_Blade";
			string strNum = to_string(iBoneIndex);

			strBoneName += strNum;

			SocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix(strBoneName.c_str());

			for (size_t i = 0; i < 3; i++)
			{
				SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
			}
			XMStoreFloat4x4(&WorldMatrix, SocketMatrix * XMLoadFloat4x4(&m_WorldMatrix));


			vCurrentPos = WorldMatrix.Translation();

			m_pEffect_Manager->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_CutScene_Weapon_Change_Insert"), vCurrentPos);

			m_isActiveInsertEffect[i] = true;
		}
	}

	if (!m_isEndPhase1Effect && m_iFrame >= 93)
	{
		m_pEffect_Manager->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Raxasia_CutScene_Weapon_Change_Fit"), m_pParentMatrix,
			m_pSocketMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

		m_isEndPhase1Effect = true;
	}
}

void CRaxasia_Sword_CutScene::Control_Phase2Effect()
{
	if (m_iFrame >= 60)
	{
		m_pModelCom->Set_SpeedRatio(m_iAnimation_Phase2Index, 2.3f);
	}
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

HRESULT CRaxasia_Sword_CutScene::Ready_Effect()
{
	m_pEffect_Manager = CEffect_Manager::Get_Instance();
	if (nullptr == m_pEffect_Manager)
		return E_FAIL;
	Safe_AddRef(m_pEffect_Manager);

	m_iTurnEffectFrame[0] = 32;
	m_iTurnEffectFrame[1] = 37;
	m_iTurnEffectFrame[2] = 38;
	m_iTurnEffectFrame[3] = 40;
	m_iTurnEffectFrame[4] = 44;
	m_iTurnEffectFrame[5] = 47;
	m_iTurnEffectFrame[6] = 49;
	m_iTurnEffectFrame[7] = 50;
	m_iTurnEffectFrame[8] = 53;
	m_iTurnEffectFrame[9] = 53;
	m_iTurnEffectFrame[10] = 54;

	m_iInsertEffectFrame[0] = 39;
	m_iInsertEffectFrame[1] = 42;
	m_iInsertEffectFrame[2] = 44;
	m_iInsertEffectFrame[3] = 47;
	m_iInsertEffectFrame[4] = 50;
	m_iInsertEffectFrame[5] = 53;
	m_iInsertEffectFrame[6] = 54;
	m_iInsertEffectFrame[7] = 57;
	m_iInsertEffectFrame[8] = 59;
	m_iInsertEffectFrame[9] = 62;
	m_iInsertEffectFrame[10] = 63;

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

	Safe_Release(m_pEffect_Manager);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}