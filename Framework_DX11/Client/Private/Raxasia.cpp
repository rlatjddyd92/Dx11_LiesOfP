#include "stdafx.h"
#include "..\Public\Raxasia.h"

#include "GameInstance.h"
#include "Player.h"
#include "Fsm.h"

#include "GameInterface_Controller.h"

#include "Effect_Manager.h"
#include "Effect_Container.h"

#pragma region Phase1

#include "State_RaxasiaP1_Idle.h"
#include "State_RaxasiaP1_Die.h"
#include "State_RaxasiaP1_Grogy.h"
#include "State_RaxasiaP1_HitFatal.h"

#include "State_RaxasiaP1_DashUpper.h"
#include "State_RaxasiaP1_GroundSlash.h"
#include "State_RaxasiaP1_LinkedAttack.h"
#include "State_RaxasiaP1_RepetUpperSlash.h"
#include "State_RaxasiaP1_KickSting.h"
#include "State_RaxasiaP1_TripleSting.h"
#include "State_RaxasiaP1_Sting_Spread.h"

#include "State_RaxasiaP1_Discharge.h"
#include "State_RaxasiaP1_JumpAttack.h"
#include "State_RaxasiaP1_SwingDown_Multiple.h"

#pragma endregion

#pragma region Phase2

#include "State_RaxasiaP2_Idle.h"
#include "State_RaxasiaP2_Die.h"
#include "State_RaxasiaP2_Grogy.h"
#include "State_RaxasiaP2_HitFatal.h"

#include "State_RaxasiaP2_JumpStamp.h"
#include "State_RaxasiaP2_Running.h"
#include "State_RaxasiaP2_StepJump.h"
#include "State_RaxasiaP2_Tele.h"
#include "State_RaxasiaP2_TeleportAttack.h"
#include "State_RaxasiaP2_WaveSting.h"
#include "State_RaxasiaP2_Guard.h"
#include "State_RaxasiaP2_Guard_Push.h"
#include "State_RaxasiaP2_ChargeRush.h"
#include "State_RaxasiaP2_Declare_War.h"
#include "State_RaxasiaP2_ShieldRush.h"
#include "State_RaxasiaP2_ShieldLinked.h"


#include "State_RaxasiaP2_Running_Linked.h"
#include "State_RaxasiaP2_Running_Fury.h"
#include "State_RaxasiaP2_Tele_JumpLightning.h"
#include "State_RaxasiaP2_Tele_LinkedTel.h"

#pragma endregion

#pragma region
#include "Raxasia_Sword_CutScene.h"
#include "State_Raxasia_CutScene_Meet.h"
#include "State_Raxasia_CutScene_Phase2.h"
#include "State_Raxasia_CutScene_Die.h"
#pragma endregion

#include "CutScene.h"
#include "Weapon.h"

#include "Dissolve_Raxasia_Dead.h"
#include "Dissolve_PowerAttack.h"

CRaxasia::CRaxasia(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CRaxasia::CRaxasia(const CRaxasia& Prototype)
	: CMonster{ Prototype }
{
}

HRESULT CRaxasia::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRaxasia::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		Desc{};
	Desc.fSpeedPerSec = 1.5f;
	Desc.fRotationPerSec = 30.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;
	
	m_eStat.strName = TEXT("Raxasia");

	m_eStat.fHp = 2000.f;
	m_eStat.fMaxHp = 2000.f;
	m_eStat.fAtk = 250.f;

	m_eStat.bWeakness = false;
	m_eStat.fGrogyPoint = 0.f;
	m_eStat.fMaxGrogyPoint = 330.f;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	if (FAILED(Ready_Weapon()))
		return E_FAIL;

	if (FAILED(Ready_Effects()))
		return E_FAIL;

	m_strObjectTag = TEXT("Monster");
	m_pColliderCom->Set_Owner(this);

	m_vCenterOffset = _Vec3{0.f, 1.85f, 0.f};

	m_eHitType = HIT_METAL;

	m_pWeapon->DeActive_Collider();
	m_pWeaponShield->DeActive_Collider();
	//m_pModelCom->SetUp_NextAnimation(12);

	GET_GAMEINTERFACE->Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_RAXASIA, this);

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);

	//Start_CutScene(CUTSCENE_MEET);

	//ChangePhase();
	return S_OK;
}

void CRaxasia::Priority_Update(_float fTimeDelta)
{
	//if (m_bChanging)
	//{
	//	m_bChanging = false;
	//}
	__super::Priority_Update(fTimeDelta);

	m_pKickCollObj->Priority_Update(fTimeDelta);
	m_pWeapon->Priority_Update(fTimeDelta);
	m_pWeaponShield->Priority_Update(fTimeDelta);

	if (!m_bDieState && m_eStat.fHp <= 0.f)
	{
		m_bDieState = true;
		//m_pFsmCom->Set_State(DIE);
		if (m_isChanged)
		{
			GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
		}

		if (m_isChanged)
		{
			dynamic_cast<CCutScene*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), BOSS1_DEAD))->Start_Play();
		}
		else
			dynamic_cast<CCutScene*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), BOSS1_PHASE2))->Start_Play();

	}

	if (m_isStartDisslove)
	{
		m_fDissloveRatio += 0.1f * fTimeDelta;
		if (m_fDissloveRatio >= 2.f)
			m_isDead = true;
	}

	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Priority_Update(fTimeDelta);
	}
	for (auto& pEffect : m_DissolveEffects)
	{
		pEffect->Priority_Update(fTimeDelta);
	}
}

void CRaxasia::Update(_float fTimeDelta)
{
	if (KEY_TAP(KEY::B))
	{
		ChangePhase();
	}
	//if (KEY_TAP(KEY::Z))
	//{
	//	m_Effects[P2_JUMPMAGIC]->Set_Loop(true);
	//}


	m_vCurRootMove = XMVector3TransformNormal(m_pModelCom->Play_Animation(fTimeDelta * m_isPlayAnimation), m_pTransformCom->Get_WorldMatrix());

	m_pRigidBodyCom->Set_Velocity(m_vCurRootMove / fTimeDelta);

	if (!m_isCutScene)
		m_pFsmCom->Update(fTimeDelta);
	else
		m_pCutSceneFsmCom->Update(fTimeDelta);


	for (_uint i = 0; i < PAWN_SOUND_END; ++i)
	{
		m_pSoundCom[i]->Update(fTimeDelta);
	}

	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Update(fTimeDelta);
	}
	for (auto& pEffect : m_DissolveEffects)
	{
		pEffect->Update(fTimeDelta);
	}

	if (!m_isCutScene)
	{
		Update_Collider();
	}

	m_pWeapon->Update(fTimeDelta);
	m_pKickCollObj->Update(fTimeDelta);
	m_pWeaponShield->Update(fTimeDelta);
}

void CRaxasia::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRigidBodyCom->Update(fTimeDelta);

	m_pWeapon->Late_Update(fTimeDelta);
	m_pKickCollObj->Late_Update(fTimeDelta);
	m_pWeaponShield->Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);

	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Late_Update(fTimeDelta);
	}
	for (auto& pEffect : m_DissolveEffects)
	{
		pEffect->Late_Update(fTimeDelta);
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

	for (_uint i = 0; i < EXCOLLIDER::COLLTYPE_END; ++i)
	{
		if(!m_isCutScene)
			m_pGameInstance->Add_ColliderList(m_EXCollider[i]);
	}
}

HRESULT CRaxasia::Render()
{
	if (FAILED(Bind_WorldViewProj()))
		return E_FAIL;

	if (FAILED(m_pDissloveTexture->Bind_ShadeResource(m_pShaderCom, "g_DissloveTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissloveRatio, sizeof(_float))))
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

		//RimLight
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimLight", &m_vRimLightColor, sizeof(_float4))))
			return E_FAIL;

		if (i == 4)	// 머리카락
		{
			if (FAILED(m_pDouTexture->Bind_ShadeResource(m_pShaderCom, "g_MaskTexture", 0)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", NORMALS, (_uint)i)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(4)))
				return E_FAIL;
		}
		else
		{
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
		}

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}
	

	//RimLight 초기화
	_Vec4 vInitRimLight = _Vec4(0.f, 0.f, 0.f, 0.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimLight", &vInitRimLight, sizeof(_float4))))
		return E_FAIL;

	_float fResetDisslove = -1.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &fResetDisslove, sizeof(_float))))
		return E_FAIL;

#ifdef _DEBUG
	m_pColliderCom->Render();
	m_pKickCollObj->Render();

	for (_int i = 0; i < COLLTYPE_END; ++i)
	{
		m_EXCollider[i]->IsActive(true);
		m_EXCollider[i]->Render();
	}
#endif

	return S_OK;
}

void CRaxasia::Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex, HIT_TYPE eHitType, ATTACK_STRENGTH eAtkStrength)
{
	if (!m_isChanged)
	{
		if (iCollIndex == 0)
		{
			m_pWeapon->Active_Collider(fDamageRatio, iCollIndex, eHitType, eAtkStrength);
		}
		else if (iCollIndex == 1)
		{
			m_pKickCollObj->Active_Collider(fDamageRatio, iCollIndex, eHitType, eAtkStrength);
		}
	}
	else
	{
		if (iCollIndex == 0)
		{
			m_pWeapon->Active_Collider(fDamageRatio, iCollIndex, eHitType, eAtkStrength);
		}
		else if (iCollIndex == 1)
		{
			m_pWeaponShield->Active_Collider(fDamageRatio, iCollIndex, eHitType, eAtkStrength);
		}
	}
}

void CRaxasia::DeActive_CurretnWeaponCollider(_uint iCollIndex)
{
	if (!m_isChanged)
	{
		if (iCollIndex == 0)
		{
			m_pWeapon->DeActive_Collider();
		}
		else if (iCollIndex == 1)
		{
			m_pKickCollObj->DeActive_Collider();
		}
	}
	else
	{
		if (iCollIndex == 0)
		{
			m_pWeapon->DeActive_Collider();
		}
		else if (iCollIndex == 1)
		{
			m_pWeaponShield->DeActive_Collider();
		}
	}
}

void CRaxasia::Active_Effect(const _uint eType, _bool isLoop)
{
	if (isLoop)
	{
		m_Effects[eType]->Set_Loop(true);
	}
	else
	{
		m_Effects[eType]->Reset_Effects();
	}
}

void CRaxasia::DeActive_Effect(const _uint eType)
{
	m_Effects[eType]->Set_Loop(false);
}

void CRaxasia::DeActive_AllEffect()
{
	for (auto& pEffect : m_Effects)
	{
		pEffect->Set_Loop(false);
	}
}

_bool CRaxasia::Get_EffectsLoop(const _uint eType)
{
	return m_Effects[eType]->Get_Dead();
}

void CRaxasia::Reset_WeaponOverlapCheck(_uint iCollIndex)
{
	m_pWeapon->Reset_OverLapCheck();
}

void CRaxasia::Change_WeaponAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration, _int iStartFrame, _bool bEitherChange, _bool bSameChange)
{
	m_pWeapon->ChangeAnimation(iAnimIndex, isLoop, fChangeDuration, iStartFrame, bEitherChange, bSameChange);
}

_bool CRaxasia::Get_WeaponAnimEnd(_int iAnimIndex)
{
	return m_pWeapon->is_EndAnim(iAnimIndex);
}

const _Matrix* CRaxasia::Get_WeaponBoneCombinedMat(_uint iBoneIndex)
{
	return m_pWeapon->Get_BoneCombinedMatrix(iBoneIndex);
}

const _Matrix* CRaxasia::Get_WeaponWorldMat()
{
	return m_pWeapon->Get_WorldMatrix_Ptr();
}

void CRaxasia::Start_CutScene(_uint iCutSceneNum)
{
	const _Matrix* pNewSocketMatrix = { nullptr };

	DeActive_AllEffect();
	m_vRimLightColor = { 0.f,0.f,0.f,0.f};

	switch (iCutSceneNum)
	{
	case CUTSCENE_MEET:
	{
		_Vec3 vOffset = _Vec3(0.07f, 0.f, 0.f);

		m_pModelCom = m_pCutSceneModelCom[MODEL_PHASE1];
		Deactiave_Weapon();

		CRaxasia_Sword_CutScene::WEAPON_DESC Desc{};
		Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("weapon0_l");
		m_pCutSceneWeapon = dynamic_cast<CRaxasia_Sword_CutScene*>(m_pGameInstance->Get_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_CutSceneWeapon"),
			TEXT("Prototype_GameObject_Weapon_Raxasia_Sword_CutScene"), &Desc));

		m_pCutSceneWeapon->Get_Transform()->Set_State(CTransform::STATE_POSITION, vOffset);

		m_pRigidBodyCom->Set_IsLockCell(false);
		m_pRigidBodyCom->Set_IsOnCell(false);

		m_pTransformCom->Rotation(_vector{ 0, 1, 0, 0 }, XMConvertToRadians(150.f));
		m_pRigidBodyCom->Set_GloblePose(XMVectorSet(-28.716f, -80.964f, -14.860f, 1.f));

		_matrix PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(180.f));
		vOffset = _Vec3(0.f, 0.1f, 0.175f);

		m_pWeaponShield->Get_Transform()->Set_State(CTransform::STATE_POSITION, vOffset);
		m_pWeaponShield->Get_Model()->Set_PreTranformMatrix(PreTransformMatrix);
		m_pWeaponShield->ChangeSocketMatrix(m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("upperArmorBack_02_spine_03"));
		m_pCutSceneFsmCom->Set_State(STATE_MEET);

		break;
	}

	case CUTSCENE_P2:
	{
		_Vec3 vOffset = _Vec3(0.f, 0.f, 0.f);

		m_pModelCom = m_pCutSceneModelCom[MODEL_PHASE1];
		Deactiave_Weapon();

		_matrix PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(-90.f));

		//CRaxasia_Sword_CutScene::WEAPON_DESC Desc{};
		//Desc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		//Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("weapon0_l");
		//m_pCutSceneWeapon = dynamic_cast<CRaxasia_Sword_CutScene*>(m_pGameInstance->Get_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_CutSceneWeapon"),
		//	TEXT("Prototype_GameObject_Weapon_Raxasia_Sword_CutScene"), &Desc));

		m_pCutSceneWeapon->Get_Model()->Set_PreTranformMatrix(PreTransformMatrix);
		m_pCutSceneWeapon->Get_Transform()->Set_State(CTransform::STATE_POSITION, vOffset);
		m_pCutSceneWeapon->Change_SocketMatrix(m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("weapon0_r"));
		m_pCutSceneWeapon->IsActive(true);

		m_pRigidBodyCom->Set_IsLockCell(false);
		m_pRigidBodyCom->Set_IsOnCell(false);

		m_pTransformCom->Rotation(_vector{ 0, 1, 0, 0 }, XMConvertToRadians(150.f));

		_Vec3 vCurrentPos = _Vec3(-59.119f, -97.802f, -27.848f);

		m_pRigidBodyCom->Set_GloblePose(vCurrentPos);

#pragma region 방패 교체
		CWeapon::MONSTER_WAPON_DESC		WeaponDesc{};
		WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("upperArmorBack_02_spine_03");	//Weapon_R UFB L
		WeaponDesc.pParentAtk = &m_eStat.fAtk;
		WeaponDesc.pMonster = this;

		Safe_Release(m_pWeaponShield);

		m_pWeaponShield = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Raxasia_P2_Shield"), &WeaponDesc));
		if (nullptr == m_pWeaponShield)
			return;

		vOffset = _Vec3(0.025f, 0.325f, 0.275f);
		PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(180.f));

		m_pWeaponShield->Get_Transform()->Set_State(CTransform::STATE_POSITION, vOffset);
		m_pWeaponShield->Get_Model()->Set_PreTranformMatrix(PreTransformMatrix);
		m_pWeaponShield->Appear();
#pragma endregion

		m_pCutSceneFsmCom->Set_State(STATE_P2);

		break;
	}

	case CUTSCENE_DIE:
	{
		_Vec3 vOffset = _Vec3(-0.17f, 0.f, 0.f);
		_matrix PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(260.f)) * XMMatrixRotationY(XMConvertToRadians(-5.f)) * XMMatrixRotationZ(XMConvertToRadians(55.f));

		m_pModelCom = m_pCutSceneModelCom[MODEL_PHASE2];

		m_pWeapon->ChangeSocketMatrix(m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("weapon0_r"));

		m_pWeaponShield->ChangeSocketMatrix(m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("weapon0_l"));
		m_pWeaponShield->Get_Transform()->Set_State(CTransform::STATE_POSITION, vOffset);
		m_pWeaponShield->Get_Model()->Set_PreTranformMatrix(PreTransformMatrix);

		
		m_pRigidBodyCom->Set_IsLockCell(false);
		m_pRigidBodyCom->Set_IsOnCell(false);

		m_pTransformCom->Rotation(_vector{ 0, 1, 0, 0 }, XMConvertToRadians(50.f));

		_Vec3 vCurrentPos = _Vec3(-59.119f, -97.743f, -27.848f);

		m_pRigidBodyCom->Set_GloblePose(vCurrentPos);

		m_pCutSceneFsmCom->Set_State(STATE_DIE);
		

		break;
	}
	}

	m_isCutScene = true;
}

void CRaxasia::End_CutScene(_uint iCutSceneNum)
{
	if (m_pCutSceneFsmCom->Get_CurrentState() == STATE_MEET)
	{
		m_pModelCom = m_pP1ModelCom;

		_Vec3 vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vCurrentPos.y -= 1.2f;

		m_pCutSceneWeapon->DeActive_AllEffect();
		m_pCutSceneWeapon->IsActive(false);
		m_pCutSceneWeapon->Start_UpdatePos();

		m_pRigidBodyCom->Set_GloblePose(vCurrentPos);
		m_pNavigationCom->Research_Cell(vCurrentPos);
		m_pRigidBodyCom->Set_IsLockCell(true);
		m_pRigidBodyCom->Set_IsOnCell(true);

		m_pWeapon->ChangeSocketMatrix(m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(62));
		Active_Weapon();

		_matrix PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) *XMMatrixRotationX(XMConvertToRadians(270.0f));
		_Vec3 vShieldOffset = _Vec3(0.f, 0.f, 0.f);

		m_pWeaponShield->Get_Transform()->Set_State(CTransform::STATE_POSITION, vShieldOffset);
		m_pWeaponShield->ChangeSocketMatrix(m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(36));
		m_pWeaponShield->Get_Model()->Set_PreTranformMatrix(PreTransformMatrix);

		m_isCutScene = false;
	}
	else if (m_pCutSceneFsmCom->Get_CurrentState() == STATE_P2)
	{
		_Vec3 vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vCurrentPos.y -= 0.8f;

		m_pRigidBodyCom->Set_GloblePose(vCurrentPos);
		m_pNavigationCom->Research_Cell(vCurrentPos);
		m_pRigidBodyCom->Set_IsLockCell(true);
		m_pRigidBodyCom->Set_IsOnCell(true);

		_matrix PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(270.0f));
		_Vec3 vShieldOffset = _Vec3(0.f, 0.f, 0.f);

		m_pCutSceneWeapon->DeActive_AllEffect();
		m_pCutSceneWeapon->IsActive(false);

		m_pWeaponShield->Get_Transform()->Set_State(CTransform::STATE_POSITION, vShieldOffset);
		m_pWeaponShield->Get_Model()->Set_PreTranformMatrix(PreTransformMatrix);

		ChangePhase();

		DeActive_Effect(CRaxasia::EFFECT_INCHENTSWORD_CUTSCENE);

		m_isCutScene = false;
	}
	else if (m_pCutSceneFsmCom->Get_CurrentState() == STATE_DIE)
	{
		m_isStartDisslove = true;
		m_DissolveEffects[DISSOLVE_DEAD]->Set_On(true);

		m_pWeapon->IsActive(false);
		m_pWeaponShield->IsActive(false);
	}

}

void CRaxasia::Change_Model(_uint iModelNum)
{
	m_pModelCom = m_pCutSceneModelCom[1];

	_uint iAnimation_Phase2 = m_pModelCom->Find_AnimationIndex("AS_Raxasia_Raxasia_Phase2_C00_CINE", 1.f);
	m_pModelCom->Set_SpeedPerSec(iAnimation_Phase2, 31.5f);
	m_pModelCom->SetUp_Animation(iAnimation_Phase2, false, 599);
	//m_pModelCom->Play_Animation(0.001f);

	m_pCutSceneWeapon->Change_SocketMatrix(m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("weapon0_r"));
	m_pWeaponShield->ChangeSocketMatrix(m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("upperArmorBack_02_spine_03")); // 나중에 바꿔야함

	//여기서 헬멧 생성
	m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("helmet_01");
}

void CRaxasia::Resetting()
{
	m_isDead = true;

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
}

HRESULT CRaxasia::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	m_pDissloveTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Anim/Player/T_DissolveMask_A.dds"), 1);

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RaxasiaP1"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pP1ModelCom))))
		return E_FAIL;

	/* FOR.Com_ExtraModel */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RaxasiaP2"),
		TEXT("Com_ExtraModel"), reinterpret_cast<CComponent**>(&m_pExtraModelCom))))
		return E_FAIL;

#pragma region CutScene
	/* FOR.Com_Model_CustScene01 */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Raxasia_CutScene_P1"),
		TEXT("Com_Model_CustScene01"), reinterpret_cast<CComponent**>(&m_pCutSceneModelCom[MODEL_PHASE1]))))
		return E_FAIL;

	/* FOR.Com_Model_CustScene02 */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Raxasia_CutScene_P2"),
		TEXT("Com_Model_CustScene02"), reinterpret_cast<CComponent**>(&m_pCutSceneModelCom[MODEL_PHASE2]))))
		return E_FAIL;
#pragma endregion
	m_pModelCom = m_pP1ModelCom;

	/* FOR.Com_Collider */		//U_Body
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.3f, 0.24f, 0.3f);
	ColliderDesc.vCenter = _float3(0.3f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	//L_Body
	ColliderDesc.vExtents = _float3(0.25f, 0.24f, 0.22f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_LowerBody"), reinterpret_cast<CComponent**>(&m_EXCollider[LOWERBODY]), &ColliderDesc)))
		return E_FAIL;

	//Leg
	{
		//U_LegL
		ColliderDesc.vExtents = _float3(0.3f, 0.15f, 0.15f);
		ColliderDesc.vCenter = _float3(0.15f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_ULL"), reinterpret_cast<CComponent**>(&m_EXCollider[U_LEG_LEFT]), &ColliderDesc)))
			return E_FAIL;

		//U_LegR
		ColliderDesc.vExtents = _float3(0.3f, 0.15f, 0.15f);
		ColliderDesc.vCenter = _float3(0.15f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_ULR"), reinterpret_cast<CComponent**>(&m_EXCollider[U_LEG_RIGHT]), &ColliderDesc)))
			return E_FAIL;

		//L_LegL
		ColliderDesc.vExtents = _float3(0.4f, 0.13f, 0.13f);
		ColliderDesc.vCenter = _float3(0.13f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_LLL"), reinterpret_cast<CComponent**>(&m_EXCollider[L_LEG_LEFT]), &ColliderDesc)))
			return E_FAIL;

		//L_LegR
		ColliderDesc.vExtents = _float3(0.4f, 0.13f, 0.13f);
		ColliderDesc.vCenter = _float3(0.13f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_LLR"), reinterpret_cast<CComponent**>(&m_EXCollider[L_LEG_RIGHT]), &ColliderDesc)))
			return E_FAIL;
	}

	//Arm
	{
		//U_ArmL
		ColliderDesc.vExtents = _float3(0.26f, 0.11f, 0.11f);
		ColliderDesc.vCenter = _float3(0.1f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_UAL"), reinterpret_cast<CComponent**>(&m_EXCollider[U_ARM_LEFT]), &ColliderDesc)))
			return E_FAIL;

		//U_ArmR
		ColliderDesc.vExtents = _float3(0.26f, 0.11f, 0.11f);
		ColliderDesc.vCenter = _float3(0.1f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_UAR"), reinterpret_cast<CComponent**>(&m_EXCollider[U_ARM_RIGHT]), &ColliderDesc)))
			return E_FAIL;

		//L_ArmL
		ColliderDesc.vExtents = _float3(0.29f, 0.1f, 0.1f);
		ColliderDesc.vCenter = _float3(0.14f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_LAL"), reinterpret_cast<CComponent**>(&m_EXCollider[L_ARM_LEFT]), &ColliderDesc)))
			return E_FAIL;

		//L_ArmR
		ColliderDesc.vExtents = _float3(0.29f, 0.1f, 0.1f);
		ColliderDesc.vCenter = _float3(0.14f, 0.f, 0.f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider_LAR"), reinterpret_cast<CComponent**>(&m_EXCollider[L_ARM_RIGHT]), &ColliderDesc)))
			return E_FAIL;
	}

	m_pColliderBindMatrix[CT_UPPERBODY] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(8);
	m_pColliderBindMatrix[CT_LOWERBODY] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(6);

	m_pColliderBindMatrix[CT_UPPERLEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(78);
	m_pColliderBindMatrix[CT_UPPERLEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(85);
	m_pColliderBindMatrix[CT_LOWERLEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(79);
	m_pColliderBindMatrix[CT_LOWERLEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(86);

	m_pColliderBindMatrix[CT_UPPERARM_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(13);
	m_pColliderBindMatrix[CT_UPPERARM_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(39);
	m_pColliderBindMatrix[CT_LOWERARM_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(14);
	m_pColliderBindMatrix[CT_LOWERARM_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(40);


	for (_uint i = 0; i < COLLTYPE_END; ++i)
		m_EXCollider[i]->Set_Owner(this);

	CRigidBody::RIGIDBODY_DESC RigidBodyDesc{};
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerTransform = m_pTransformCom;
	RigidBodyDesc.pOwnerNavigation = m_pNavigationCom;
	RigidBodyDesc.isCapsule = true;

	RigidBodyDesc.pOwner = this;
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestituion = 0.f;

	physX::GeometryCapsule CapsuleDesc;
	CapsuleDesc.fHeight = 2.5f;
	CapsuleDesc.fRadius = 0.45f;
	RigidBodyDesc.pGeometry = &CapsuleDesc;
	RigidBodyDesc.PxLockFlags = PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;

	physX::GeometryTriangleMesh TriangleDesc;
	TriangleDesc.pModel = m_pModelCom;
	RigidBodyDesc.pGeometry = &TriangleDesc;

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidBodyDesc)))
		return E_FAIL;

	m_pRigidBodyCom->Set_Mass(100000.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RaxasiaDOU"), 
		TEXT("Com_DouTexture"), reinterpret_cast<CComponent**>(&m_pDouTexture))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRaxasia::Ready_FSM()
{
	if (FAILED(__super::Ready_FSM()))
		return E_FAIL;

	FSM_INIT_DESC Desc{};



#pragma region Phase1_Fsm
	m_pFsmCom->Add_State(CState_RaxasiaP1_Idle::Create(m_pFsmCom, this, IDLE, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_Grogy::Create(m_pFsmCom, this, GROGY, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_HitFatal::Create(m_pFsmCom, this, HITFATAL, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_Die::Create(m_pFsmCom, this, DIE, &Desc));

	m_pFsmCom->Add_State(CState_RaxasiaP1_DashUpper::Create(m_pFsmCom, this, ATK_DASHUPPER, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_GroundSlash::Create(m_pFsmCom, this, ATK_GROUNDSLASH, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_LinkedAttack::Create(m_pFsmCom, this, ATK_LINKEDATTACK, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_RepetUpperSlash::Create(m_pFsmCom, this, ATK_REPETUPPERSLASH, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_KickSting::Create(m_pFsmCom, this, ATK_KICKSTING, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_TripleSting::Create(m_pFsmCom, this, ATK_TRIPLESTING, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_Sting_Spread::Create(m_pFsmCom, this, ATK_STING_ANDSPREAD, &Desc));

	m_pFsmCom->Add_State(CState_RaxasiaP1_Discharge::Create(m_pFsmCom, this, ATK_DISCHARGE, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_JumpAttack::Create(m_pFsmCom, this, ATK_JUMPATTACK, &Desc));
	m_pFsmCom->Add_State(CState_RaxasiaP1_SwingDown_Multiple::Create(m_pFsmCom, this, ATK_SWINGDOWN_MULTIPLE, &Desc));


	m_pFsmCom->Set_State(IDLE);
#pragma endregion

#pragma region Phase2_Fsm

	/* FOR.Com_FSM */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		TEXT("Com_ExtraFSM"), reinterpret_cast<CComponent**>(&m_pExtraFsmCom))))
		return E_FAIL;

	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Idle::Create(m_pExtraFsmCom, this, IDLE, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Grogy::Create(m_pExtraFsmCom, this, GROGY, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_HitFatal::Create(m_pExtraFsmCom, this, HITFATAL, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Die::Create(m_pExtraFsmCom, this, DIE, &Desc));

	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_JumpStamp::Create(m_pExtraFsmCom, this, ATKP2_JUMPSTAMP, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_StepJump::Create(m_pExtraFsmCom, this, ATKP2_STEPJUMP, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Running::Create(m_pExtraFsmCom, this, ATKP2_RUNNING, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Tele::Create(m_pExtraFsmCom, this, ATKP2_TELE, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_TeleportAttack::Create(m_pExtraFsmCom, this, ATKP2_TELEPORTATTACK, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_WaveSting::Create(m_pExtraFsmCom, this, ATKP2_WAVESTING, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Guard::Create(m_pExtraFsmCom, this, ATKP2_GUARD, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_ChargeRush::Create(m_pExtraFsmCom, this, ATKP2_CHARGERUSH, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Declare_War::Create(m_pExtraFsmCom, this, ATKP2_DECLAREWAR, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_ShieldRush::Create(m_pExtraFsmCom, this, ATKP2_SHIELDRUSH, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_ShieldLinked::Create(m_pExtraFsmCom, this, ATKP2_SHIELDLINKED, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Guard_Push::Create(m_pExtraFsmCom, this, ATKP2_GUARD_PUSH, &Desc));

	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Running_Linked::Create(m_pExtraFsmCom, this, ATKP2_RUNNING_LINKED, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Running_Fury::Create(m_pExtraFsmCom, this, ATKP2_RUNNING_FURY, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Tele_LinkedTel::Create(m_pExtraFsmCom, this, ATKP2_TELE_LINKEDTEL, &Desc));
	m_pExtraFsmCom->Add_State(CState_RaxasiaP2_Tele_JumpLightning::Create(m_pExtraFsmCom, this, ATKP2_TELE_JUMPLIGHTNING, &Desc));

#pragma endregion

#pragma region CutScene_Fsm
	/* FOR.Com_FSM */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		TEXT("Com_CutSCeneFSM"), reinterpret_cast<CComponent**>(&m_pCutSceneFsmCom))))
		return E_FAIL;

	m_pCutSceneFsmCom->Add_State(CState_Raxasia_CutScene_Meet::Create(m_pCutSceneFsmCom, this, STATE_MEET, &Desc));
	m_pCutSceneFsmCom->Add_State(CState_Raxasia_CutScene_Phase2::Create(m_pCutSceneFsmCom, this, STATE_P2, &Desc));
	m_pCutSceneFsmCom->Add_State(CState_Raxasia_CutScene_Die::Create(m_pCutSceneFsmCom, this, STATE_DIE, &Desc));
#pragma endregion

	return S_OK;
}

HRESULT CRaxasia::Ready_Weapon()
{
	CWeapon::MONSTER_WAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(62);	//Weapon_R

	WeaponDesc.pParentAtk = &m_eStat.fAtk;

	WeaponDesc.pMonster = this;

	m_pWeapon = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Raxasia_P1_Sword"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return E_FAIL;

	m_pWeapon->Appear();
	m_pWeapon->DeActive_Collider();


	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(36);	//

	WeaponDesc.pParentAtk = &m_eStat.fAtk;

	m_pWeaponShield = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Raxasia_P2_Shield"), &WeaponDesc));
	if (nullptr == m_pWeaponShield)
		return E_FAIL;

	m_pWeaponShield->Appear();
	m_pWeaponShield->DeActive_Collider();


	/* FOR.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderOBBDesc_Obj{};

	ColliderOBBDesc_Obj.vAngles = _float3(0.f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vCenter = _float3(0.2f, 0.f, 0.f);
	ColliderOBBDesc_Obj.vExtents = _float3(0.6f, 0.2f, 0.2f);

	CColliderObject::COLIDEROBJECT_DESC Desc{};

	Desc.pBoundingDesc = &ColliderOBBDesc_Obj;
	Desc.eType = CCollider::TYPE_OBB;
	Desc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_FOOT_RIGHT) - 1);
	Desc.pParentWorldMatrix = WeaponDesc.pParentWorldMatrix;
	Desc.pSocketBoneMatrix2 = WeaponDesc.pParentWorldMatrix;
	Desc.fDamageAmount = m_eStat.fAtk;

	m_pKickCollObj = dynamic_cast<CColliderObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ColliderObj"), &Desc));



	return S_OK;
}

HRESULT CRaxasia::Ready_Effects()
{
	m_Effects.resize(EFFECT_END);

	const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	const _Matrix* pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_ROOT));

	
	m_Effects[EFFECT_DASH] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_Dash"), pParetnMatrix,
		nullptr, _Vec3(0.f, 2.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_HOWLING] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_Howling"), pParetnMatrix,
		nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_THUNDERCHARGE_GROUND] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderCharge_Ground"), pParetnMatrix,
		nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_THUNDERACCEL] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderAccel"), pParetnMatrix,
		nullptr, _Vec3(0.f, 0.2f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_SHIELDDASH] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ShieldDash"), pParetnMatrix,
		nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_SHIELDCHARGE_GROUND] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ShieldCharge_Ground"), pParetnMatrix,
		nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_THUNDERDISCHARGE] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderDischarge"), pParetnMatrix,
		nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_WEAPON));

	m_Effects[EFFECT_SWING] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_Swing"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_DRAG] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_Drag"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_THUNDERDRAG] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderDrag"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_INCHENTSWORD] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_InchentedSword"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	pSocketBoneMatrix = m_pExtraModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pExtraModelCom->Get_UFBIndices(UFB_CHEST));

	m_Effects[EFFECT_THUNDERENVELOP_BIG] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderEnvelop_Big"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));


	pSocketBoneMatrix = m_pExtraModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pExtraModelCom->Get_UFBIndices(UFB_WEAPON));

	m_Effects[EFFECT_INCHENTSWORD_P2] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_InchentedSword_2P"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));


	pSocketBoneMatrix = m_pExtraModelCom->Get_BoneCombindTransformationMatrix_Ptr(8);

	m_Effects[EFFECT_THUNDERENVELOP_SMALL] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderEnvelop_Small"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f), _Vec3(60.f, 0.f, -90.f));


	m_Effects[EFFECT_INCHENTSWORD_CUTSCENE] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_InchentedSword_CutScene"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f), _Vec3(0.f, 0.f, 0.f));
	

	m_DissolveEffects.resize(DISSOLVE_END);

	CDissolve_Raxasia_Dead::DISSOLVE_OBJECT_DESC TestDesc = {};
	TestDesc.fRotationPerSec = 90.f;
	TestDesc.fSpeedPerSec = 1.f;
	TestDesc.iLevelIndex = LEVEL_GAMEPLAY;
	TestDesc.pTarget_ModelCom = m_pCutSceneModelCom[MODEL_PHASE2];
	TestDesc.pTarget_TransformCom = m_pTransformCom;
	TestDesc.pDissolveTextureCom = m_pDissloveTexture;
	TestDesc.pThreshold = &m_fDissloveRatio;
	TestDesc.vTextureSize = _float2(2048.f, 2048.f);
	TestDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Dissolve_Raxasia_Dead");

	m_DissolveEffects[DISSOLVE_DEAD] = static_cast<CDissolve_Raxasia_Dead*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Dissolve_Raxasia_Dead"), &TestDesc));

	CDissolve_Effect::DISSOLVE_EFFECT_DESC PowerAttackDesc = {};
	PowerAttackDesc.fRotationPerSec = XMConvertToRadians(90.f);
	PowerAttackDesc.fSpeedPerSec = 1.f;
	PowerAttackDesc.iLevelIndex = LEVEL_GAMEPLAY;
	PowerAttackDesc.pTarget_ModelCom = m_pP1ModelCom;
	PowerAttackDesc.pTarget_TransformCom = m_pTransformCom;
	PowerAttackDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Dissolve_Raxasia_P1_PowerAttack");
	m_DissolveEffects[DISSOLVE_POWERATTACK_P1] = static_cast<CDissolve_PowerAttack*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Dissolve_PowerAttack"), &PowerAttackDesc));

	PowerAttackDesc.pTarget_ModelCom = m_pExtraModelCom;
	PowerAttackDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Dissolve_Raxasia_P2_PowerAttack");
	m_DissolveEffects[DISSOLVE_POWERATTACK_P2] = static_cast<CDissolve_PowerAttack*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Dissolve_PowerAttack"), &PowerAttackDesc));

	m_DissolveEffects[DISSOLVE_POWERATTACK_P1]->Set_On(true);
	return S_OK;
}

void CRaxasia::Update_Collider()
{
	_float4x4 UpdateMat{};

	_Matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_UPPERBODY]) * WorldMat);
	m_pColliderCom->Update(&UpdateMat);

	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LOWERBODY]) * WorldMat);
	m_EXCollider[LOWERBODY]->Update(&UpdateMat);


	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_UPPERLEG_LEFT]) * WorldMat);
	m_EXCollider[U_LEG_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_UPPERLEG_RIGHT]) * WorldMat);
	m_EXCollider[U_LEG_RIGHT]->Update(&UpdateMat);
	
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LOWERLEG_LEFT]) * WorldMat);
	m_EXCollider[L_LEG_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LOWERLEG_RIGHT]) * WorldMat);
	m_EXCollider[L_LEG_RIGHT]->Update(&UpdateMat);


	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_UPPERARM_LEFT]) * WorldMat);
	m_EXCollider[U_ARM_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_UPPERARM_RIGHT]) * WorldMat);
	m_EXCollider[U_ARM_RIGHT]->Update(&UpdateMat);

	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LOWERARM_LEFT]) * WorldMat);
	m_EXCollider[L_ARM_LEFT]->Update(&UpdateMat);
	XMStoreFloat4x4(&UpdateMat
		, *(m_pColliderBindMatrix[CT_LOWERARM_RIGHT]) * WorldMat);
	m_EXCollider[L_ARM_RIGHT]->Update(&UpdateMat);


}

void CRaxasia::ChangePhase()
{
	if (m_pExtraModelCom == nullptr || m_pExtraFsmCom == nullptr)
	{
		return;
	}

	m_pFsmCom->Release_States();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pFsmCom);

	m_pModelCom = m_pExtraModelCom;
	m_pFsmCom = m_pExtraFsmCom;
	
	m_pExtraModelCom = nullptr;
	m_pExtraFsmCom = nullptr;

	m_pModelCom->SetUp_Animation(8, true);//P2 Idle
	m_pFsmCom->Set_State(IDLE);

	m_pModelCom->Play_Animation(0);
	
	//이펙트 재조정

	const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	const _Matrix* pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_WEAPON));

	Safe_Release(m_Effects[EFFECT_SWING]);
	Safe_Release(m_Effects[EFFECT_DRAG]);
	Safe_Release(m_Effects[EFFECT_THUNDERDRAG]);
	Safe_Release(m_Effects[EFFECT_INCHENTSWORD]);

	m_Effects[EFFECT_SWING] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_Swing"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_DRAG] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_Drag"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_THUNDERDRAG] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_ThunderDrag"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_INCHENTSWORD] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Raxasia_Attack_InchentedSword"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 1.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_INCHENTSWORD_P2]->Set_Matrices(pSocketBoneMatrix);

	//P2

	CWeapon::MONSTER_WAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pParentAtk = &m_eStat.fAtk;
	WeaponDesc.pMonster = this;
	WeaponDesc.pSocketBoneMatrix = pSocketBoneMatrix;
	Safe_Release(m_pWeapon);

	m_pWeapon = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Raxasia_P2_Sword"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return;

	m_pWeapon->Appear();
	m_pWeapon->DeActive_Collider();


	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(46);

	m_pWeaponShield->ChangeSocketMatrix(WeaponDesc.pSocketBoneMatrix);


	m_pColliderBindMatrix[CT_UPPERBODY] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(8);//Spine 2
	m_pColliderBindMatrix[CT_LOWERBODY] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(6);//Spine

	m_pColliderBindMatrix[CT_UPPERLEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(124);//Tight
	m_pColliderBindMatrix[CT_UPPERLEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(129);
	m_pColliderBindMatrix[CT_LOWERLEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(125);//Calf
	m_pColliderBindMatrix[CT_LOWERLEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(130);

	m_pColliderBindMatrix[CT_UPPERARM_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(23);//UpperArm
	m_pColliderBindMatrix[CT_UPPERARM_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(70);
	m_pColliderBindMatrix[CT_LOWERARM_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(24);//ForeArm
	m_pColliderBindMatrix[CT_LOWERARM_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(71);

	m_eStat.fHp = 2500.f;
	m_eStat.fHp = 1.f;
	m_eStat.fMaxHp = 2500.f;
	m_eStat.fAtk = 250.f;
	//m_eStat.fDefence = 8.f;

	m_eStat.fMaxGrogyPoint = 330.f;
	m_eStat.fGrogyPoint = 0.f;

	m_bDieState = false;
	m_isDead = false;
	m_isChanged = true;
}

void CRaxasia::Change_Phase2Sword()
{
	if (nullptr != m_pWeapon)
		Safe_Release(m_pWeapon);

	CWeapon::MONSTER_WAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("weapon0_r");	//Weapon_R

	WeaponDesc.pParentAtk = &m_eStat.fAtk;

	WeaponDesc.pMonster = this;

	m_pWeapon = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Raxasia_P2_Sword"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return;

	// 실험해보기
	m_Effects[EFFECT_INCHENTSWORD_CUTSCENE]->Set_Matrices(WeaponDesc.pSocketBoneMatrix);
	Active_Effect(CRaxasia::EFFECT_INCHENTSWORD_CUTSCENE, true);

	m_pWeapon->Appear();
}

CRaxasia* CRaxasia::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRaxasia* pInstance = new CRaxasia(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSimonManus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CRaxasia::Clone(void* pArg)
{
	CRaxasia* pInstance = new CRaxasia(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CRaxasia"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRaxasia::Free()
{
	for (_uint i = 0; i < COLLTYPE_END; ++i)
	{
		Safe_Release(m_EXCollider[i]);
	}

	Safe_Release(m_pDissloveTexture);
	Safe_Release(m_pWeapon);
	Safe_Release(m_pWeaponShield);
	Safe_Release(m_pP1ModelCom);
	Safe_Release(m_pExtraModelCom);
	Safe_Release(m_pKickCollObj);

	if(m_pModelCom != m_pCutSceneModelCom[0])
		Safe_Release(m_pCutSceneModelCom[0]);
	if (m_pModelCom != m_pCutSceneModelCom[1])
		Safe_Release(m_pCutSceneModelCom[1]);


	Safe_Release(m_pDouTexture);

	if (true == m_isCloned)
	{
		for (auto& pEffect : m_Effects)
		{
			pEffect->Set_Cloned(false);
			Safe_Release(pEffect);
		}
		m_Effects.clear();
	}

	if (m_pExtraFsmCom != nullptr)
	{
		m_pExtraFsmCom->Release_States();
	}
	Safe_Release(m_pExtraFsmCom);

	if (m_pCutSceneFsmCom != nullptr)
	{
		m_pCutSceneFsmCom->Release_States();
	}
	Safe_Release(m_pCutSceneFsmCom);
	
	for (auto& DissolveEffect : m_DissolveEffects)
		Safe_Release(DissolveEffect);
	m_DissolveEffects.clear();

	__super::Free();

}