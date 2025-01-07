#include "stdafx.h"
#include "..\Public\SimonManus.h"

#include "GameInstance.h"
#include "Player.h"
#include "Fsm.h"

#include "GameInterface_Controller.h"

#include "Effect_Manager.h"
#include "Effect_Container.h"

#include "Weapon.h"
#include "CutScene.h"

#pragma region Phase1
#include "State_SimonManusP1_Idle.h"
#include "State_SimonManusP1_Die.h"
#include "State_SimonManusP1_Grogy.h"
#include "State_SimonManusP1_HitFatal.h"

#include "State_SimonManusP1_StingAttack.h"
#include "State_SimonManusP1_Stamp.h"
#include "State_SimonManusP1_ChasingSwing.h"
#include "State_SimonManusP1_JumpToSwing.h"
#include "State_SimonManusP1_HighJumpFall.h"
#include "State_SimonManusP1_SwingMultiple.h"
#include "State_SimonManusP1_SwipMultiple_R.h"
#include "State_SimonManusP1_SwipMultiple_L.h"
#include "State_SimonManusP1_SwingDown_Swing_R.h"
#include "State_SimonManusP1_SwingDown_Swing_L.h"
#include "State_SimonManusP1_Charge_SwingDown.h"
#pragma endregion

#pragma region Phase2
#include "State_SimonManusP2_Idle.h"
#include "State_SimonManusP2_Die.h"
#include "State_SimonManusP2_Grogy.h"
#include "State_SimonManusP2_HitFatal.h"

#include "State_SimonManusP2_ChasingSwing.h"
#include "State_SimonManusP2_BackLightning.h"
#include "State_SimonManusP2_SwingDown_Swing.h"
#include "State_SimonManusP2_Stamp.h"
#include "State_SimonManusP2_SwipMultiple.h"
#include "State_SimonManusP2_JumpToAttack.h"
#include "State_SimonManusP2_Route1.h"
#include "State_SimonManusP2_Route2.h"

#include "State_SimonManusP2_Sting.h"
#include "State_SimonManusP2_BrutalAttack.h"
#include "State_SimonManusP2_SummonHand.h"
#include "State_SimonManusP2_HighJumpFall.h"
#include "State_SimonManusP2_ThunderBall.h"
#include "State_SimonManusP2_ThunderCalling.h"
#include "State_SimonManusP2_Wave.h"
#include "State_SimonManusP2_SpreadMagic.h"
#include "State_SimonManusP2_SlideMagic.h"

#include "State_SimonManusP2_Die_Talking.h"
#pragma endregion

#pragma region CutScene
#include "State_SimonManus_CutScene_Meet.h"
#include "State_SimonManus_CutScene_Phase2.h"
#pragma endregion

// 고준호
#include "SimonManus_2P_Aura.h"
#include "Dissolve_SimonManus_Dead.h"
#include "Dissolve_PowerAttack.h"


CSimonManus::CSimonManus(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster{ pDevice, pContext }
{
}

CSimonManus::CSimonManus(const CSimonManus& Prototype)
	: CMonster{ Prototype }
{
}

HRESULT CSimonManus::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSimonManus::Initialize(void* pArg)
{
  	CGameObject::GAMEOBJECT_DESC		Desc{};
	Desc.fSpeedPerSec = 1.5f;
	Desc.fRotationPerSec = 30.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->LookAt(_vector{ 0, 0, -1, 0 });

	m_eStat.strName = TEXT("시몬 마누스");

	m_eStat.fHp = 3000.f;
	m_eStat.fMaxHp = 3000.f;
	m_eStat.fAtk = 270.f;
	//m_eStat.fDefence = 5.f;

	m_eStat.bWeakness = false;
	m_eStat.fGrogyPoint = 0.f;
	m_eStat.fMaxGrogyPoint = 390.f;

	m_iErgoPoint = 700;

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

	for (_uint i = 0; i < EXCOLLIDER::COLLTYPE_END; ++i)
	{
		m_EXCollider[i]->Set_Owner(this);
	}

	m_isBoss = true;

	m_vCenterOffset = _Vec3{ 0.f, 1.95f, 0.f };

	m_eHitType = HIT_CARCASS;

	m_pWeapon->DeActive_Collider();

	m_pTransformCom->Rotation(0.f, 50.f, 0.f);

	GET_GAMEINTERFACE->Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_SIMON, this);

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);

	if (dynamic_cast<CCutScene*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), BOSS2_MEET))->Get_bHavePlayed())
	{
		m_isFirstCreate = false;
		m_pRigidBodyCom->Set_GloblePose(_Vec3(0.f, 0.f, 0.f));
		m_pTransformCom->LookAt_NoHeight(static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY))->Get_Transform()->Get_State(CTransform::STATE_POSITION));
	}
	else
	{
		m_isFirstCreate = true;
		Start_CutScene(CUTSCENE_MEET);
	}

	//m_pNavigationCom->Set_ExceptCellNum(99);

	return S_OK;
}

void CSimonManus::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	m_pWeapon->Priority_Update(fTimeDelta);

	if (!m_bDieState && m_eStat.fHp <= 0.f)
	{
		m_bDieState = true;
		m_pWeapon->DeActive_Collider();

		if(m_isChanged)
			dynamic_cast<CCutScene*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), BOSS2_DEFEAT))->Start_Play();
		else
			dynamic_cast<CCutScene*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), BOSS2_PHASE2))->Start_Play();

		if (m_isChanged)
		{
			GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
		}

	}

	if (m_isStartDisslove)
	{
		m_fDissloveRatio += 0.3f * fTimeDelta;
		if (m_fDissloveRatio >= 2.f)
		{
			m_isDead = true;
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
			pPlayer->Get_Player_Stat_Adjust()->iErgo += m_iErgoPoint;
			pPlayer->Set_SophiaDeadCutScene();
		}
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

void CSimonManus::Update(_float fTimeDelta)
{
	if (KEY_TAP(KEY::B))
	{
		SetUp_Act();
	}

	if (m_isPlayAnimation)
	{
		m_vCurRootMove = XMVector3TransformNormal(m_pModelCom->Play_Animation(fTimeDelta), m_pTransformCom->Get_WorldMatrix());
	}
	else
	{
		m_vCurRootMove = _Vec3(0.f, 0.f, 0.f);
	}

	m_pRigidBodyCom->Set_Velocity(m_vCurRootMove / fTimeDelta);

	if(!m_isCutScene)
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
		Update_Debuff(fTimeDelta);
	}

	m_pWeapon->Update(fTimeDelta);
	
}

void CSimonManus::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRigidBodyCom->Update(fTimeDelta);
	
	m_pWeapon->Late_Update(fTimeDelta);

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
		m_pGameInstance->Add_ColliderList(m_EXCollider[i]);
	}
}

HRESULT CSimonManus::Render()
{
	_bool isCutSceneRender = false;

	if (m_pModelCom == m_pCutSceneModelCom[0] || m_pModelCom == m_pCutSceneModelCom[1])
	{
		isCutSceneRender = true;
	}

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

		if (m_pModelCom == m_pCutSceneModelCom[0] && (i == 8 || i == 10))
		{
			if (FAILED(m_pShaderCom->Begin(5)))
				return E_FAIL;
		}
		else if (m_pModelCom == m_pCutSceneModelCom[1] && (i == 2 || i == 3))
		{
			if (FAILED(m_pShaderCom->Begin(5)))
				return E_FAIL;
		}
		else
		{
			if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::ROUGHNESS))
			{
				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ARMTexture", ROUGHNESS, (_uint)i)))
					return E_FAIL;
			}

			_float fEmissiveMaks = 0.f;
			// EMISSIVE
			if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::EMISSIVE))
			{
				fEmissiveMaks = m_fEmissiveMask;
				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmessiveTexture", EMISSIVE, (_uint)i)))
					return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmessiveMask", &fEmissiveMaks, sizeof(_float))))
				return E_FAIL;

			//RimLight
			if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimLight", &m_vRimLightColor, sizeof(_float4))))
				return E_FAIL;


			if (m_isChanged)
			{
				if (!isCutSceneRender &&(i == 1 || i == 2))
				{
					if (FAILED(m_pShaderCom->Begin(5)))
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
			}
			else
			{
				if (!isCutSceneRender && (i == 3 || i == 5))
				{
					if (FAILED(m_pShaderCom->Begin(5)))
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
	m_EXCollider[LEG_LEFT]->Render();
	m_EXCollider[LEG_RIGHT]->Render();
	m_EXCollider[LOWERBODY]->Render();
#endif

	return S_OK;
}

void CSimonManus::Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex, HIT_TYPE eHitType, ATTACK_STRENGTH eAtkStrength)
{
	m_pWeapon->Active_Collider(fDamageRatio, iCollIndex, eHitType, eAtkStrength);
}

void CSimonManus::DeActive_CurrentWeaponCollider(_uint iCollIndex)
{
	m_pWeapon->DeActive_Collider();
}

void CSimonManus::Start_StartDisslove()
{
	m_isStartDisslove = true;
	m_DissolveEffects[DISSOLVE_DEAD]->Set_On(true);
}

void CSimonManus::Active_Effect(const _uint eType, _bool isLoop)
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

void CSimonManus::DeActive_Effect(const _uint eType)
{
	m_Effects[eType]->Set_Loop(false);
}

void CSimonManus::DeActive_AllEffect()
{
	for (auto& Effect : m_Effects)
	{
		Effect->Set_Loop(false);
	}
}

void CSimonManus::Set_Dead_Effect(const _uint eType)
{
	m_Effects[eType]->Set_Dead(true);
}

_bool CSimonManus::Get_EffectsLoop(const _uint eType)
{
	return m_Effects[eType]->Get_Dead();
}

void CSimonManus::Reset_WeaponOverlapCheck(_uint iCollIndex)
{
	m_pWeapon->Reset_OverLapCheck();
}

void CSimonManus::Change_WeaponAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration, _int iStartFrame, _bool bEitherChange, _bool bSameChange)
{
	m_pWeapon->ChangeAnimation(iAnimIndex, isLoop, fChangeDuration, iStartFrame, bEitherChange, bSameChange);
}

_bool CSimonManus::Get_WeaponAnimEnd(_int iAnimIndex)
{
	return m_pWeapon->is_EndAnim(iAnimIndex);
}

const _Matrix* CSimonManus::Get_WeaponBoneCombinedMat(_uint iBoneIndex)
{
	return m_pWeapon->Get_BoneCombinedMatrix(iBoneIndex);
}

const _Matrix* CSimonManus::Get_WeaponWorldMat()
{
	return m_pWeapon->Get_WorldMatrix_Ptr();
}

void CSimonManus::Start_CutScene(_uint iCutSceneNum)
{
	const _Matrix* pNewSocketMatrix = { nullptr };

	DeActive_AllEffect();

	switch (iCutSceneNum)
	{
	case CUTSCENE_MEET :
		m_pModelCom = m_pCutSceneModelCom[MODEL_PHASE1];

		pNewSocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("Bn_Weapon_R");
		m_pWeapon->ChangeSocketMatrix(pNewSocketMatrix);
		Deactiave_Weapon();

		m_pCutSceneFsmCom->Set_State(STATE_MEET);
		break;
	case CUTSCENE_P2:
		m_pModelCom = m_pCutSceneModelCom[MODEL_PHASE1];

		pNewSocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("Bn_Weapon_R");
		m_pWeapon->ChangeSocketMatrix(pNewSocketMatrix);
		Deactiave_Weapon();	// 수정해보기

		m_isPlayAnimation = true;

		m_pRigidBodyCom->Set_GloblePose(_Vec3(0.f, 0.f, 0.f));
		m_pTransformCom->Rotation(0.f, 50.f, 0.f);

		if(m_isFirstCreate)
			m_pCutSceneFsmCom->Change_State(STATE_P2);
		else
			m_pCutSceneFsmCom->Set_State(STATE_P2);

		break;
	case CUTSCENE_DIE:
		m_pModelCom = m_pCutSceneModelCom[MODEL_PHASE2];
		break;
	}

	m_isCutScene = true;
}

void CSimonManus::End_CutScene(_uint iCutSceneNum)
{
	if (iCutSceneNum == STATE_MEET)
	{
		m_pModelCom = m_pP1ModelCom;

		const _Matrix* pNewSocketMatrix = { nullptr };
		pNewSocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("Bn_Weapon_R");
		m_pWeapon->ChangeSocketMatrix(pNewSocketMatrix);
		
		Active_Weapon();
		SetUp_Act();
	}
	else if (iCutSceneNum == STATE_P2)
	{
		ChangePhase();
		Active_Weapon();
	}
	else if (iCutSceneNum == STATE_DIE)
	{
		m_pWeapon->IsActive(false);
		Change_State(CSimonManus::DIE_TALKING);
	}

	m_isCutScene = false;
}

void CSimonManus::Change_Model(_uint iModelNum)	// 컷신 2페이즈로 바꾸는 용도
{
	m_pModelCom = m_pCutSceneModelCom[1];
	Deactiave_Weapon();

	const _Matrix* pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("upperBody_spine_01");

	m_Effects[CUTSCENE_P2_ARM_PARTICLE]->Set_Matrices(pSocketBoneMatrix);

}

void CSimonManus::On_Aura(_bool bAura, _bool bCutScene)
{
	m_DissolveEffects[DISSOLVE_AURA]->Set_On(bAura);
	static_cast<CSimonManus_2P_Aura*>(m_DissolveEffects[DISSOLVE_AURA])->Set_CutScene(bCutScene);
}

void CSimonManus::Resetting()
{
	m_isDead = true;

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
}

void CSimonManus::On_PowerAttack(_bool bOn)
{
	if (false == m_isChanged)
		m_DissolveEffects[DISSOLVE_POWERATTACK_P1]->Set_On(bOn);
	else
		m_DissolveEffects[DISSOLVE_POWERATTACK_P2]->Set_On(bOn);
}

void CSimonManus::Update_Debuff(_float fTimeDelta)
{
	for (_uint i = 0; i < DEBUFF_END; ++i)
	{
		if (m_bDebuffed[i])
		{
			if (m_fDebuffDuration[i] > fTimeDelta)
			{
				m_fDebuffDuration[i] -= fTimeDelta;
				m_eStat.fHp -= m_eStat.fMaxHp * 0.01f * fTimeDelta;
			}
			else
			{
				m_eStat.fHp -= m_eStat.fMaxHp * 0.01f * m_fDebuffDuration[i];
				m_fDebuffDuration[i] = 0.f;
			}

		}
		//이펙트 업데이트

	}
}

HRESULT CSimonManus::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	// clone으로 바꿀까?
	m_pDissloveTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Anim/Player/T_DissolveMask_A.dds"), 1);

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManusP1"),
		TEXT("Com_P1Model"), reinterpret_cast<CComponent**>(&m_pP1ModelCom))))
		return E_FAIL;

	/* FOR.Com_ExtraModel */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManusP2"),
		TEXT("Com_ExtraModel"), reinterpret_cast<CComponent**>(&m_pExtraModelCom))))
		return E_FAIL;

#pragma region 컷신용
	/* FOR.Com_Model_CustScene01 */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManus_CutScene_P1"),
		TEXT("Com_Model_CustScene01"), reinterpret_cast<CComponent**>(&m_pCutSceneModelCom[MODEL_PHASE1]))))
		return E_FAIL;

	/* FOR.Com_Model_CustScene02 */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManus_CutScene_P2"),
		TEXT("Com_Model_CustScene02"), reinterpret_cast<CComponent**>(&m_pCutSceneModelCom[MODEL_PHASE2]))))
		return E_FAIL;
#pragma endregion
	m_pModelCom = m_pP1ModelCom;

	/* FOR.Com_Collider */		//Body
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(1.3f, 1.3f, 1.7f);
	ColliderDesc.vCenter = _float3(1.4f, 0.3f, 0.35f);
	ColliderDesc.vAngles = _float3(-0.3f, 0.f, 0.2f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	//LegsL
	ColliderDesc.vExtents = _float3(0.3f, 0.8f, 0.3f);
	ColliderDesc.vCenter = _float3(-0.1f, -0.4f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, -0.45f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_LL"), reinterpret_cast<CComponent**>(&m_EXCollider[LEG_LEFT]), &ColliderDesc)))
		return E_FAIL;

	//R
	ColliderDesc.vExtents = _float3(0.3f, 0.8f, 0.3f);
	ColliderDesc.vCenter = _float3(-0.1f, -0.4f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, -0.45f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_RL"), reinterpret_cast<CComponent**>(&m_EXCollider[LEG_RIGHT]), &ColliderDesc)))
		return E_FAIL;

	//LowerBody
	ColliderDesc.vExtents = _float3(0.8f, 1.f, 0.9f);
	ColliderDesc.vCenter = _float3(0.f, -0.1f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_LowerBody"), reinterpret_cast<CComponent**>(&m_EXCollider[LOWERBODY]), &ColliderDesc)))
		return E_FAIL;

	m_pColliderBindMatrix[CT_LEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(112);
	m_pColliderBindMatrix[CT_LEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(126);
	m_pColliderBindMatrix[CT_UPPERBODY] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(6);
	m_pColliderBindMatrix[CT_LOWERBODY] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(5);

	for (_uint i = 0; i < COLLTYPE_END; ++i)
		m_EXCollider[i]->Set_Owner(this);
		

	// 항상 마지막에 생성하기
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
	CapsuleDesc.fRadius = 0.9f;
	RigidBodyDesc.pGeometry = &CapsuleDesc;
	RigidBodyDesc.PxLockFlags = PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidBodyDesc)))
		return E_FAIL;
	return S_OK;
	m_pRigidBodyCom->Set_Mass(100000.f);

	return S_OK;
}

HRESULT CSimonManus::Ready_FSM()
{
	if (FAILED(__super::Ready_FSM()))
		return E_FAIL;

	FSM_INIT_DESC Desc{};
	


#pragma region Phase1_Fsm
	m_pFsmCom->Add_State(CState_SimonManusP1_Idle::Create(m_pFsmCom, this, IDLE, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_Grogy::Create(m_pFsmCom, this, GROGY, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_HitFatal::Create(m_pFsmCom, this, HITFATAL, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_Die::Create(m_pFsmCom, this, DIE, &Desc));

	m_pFsmCom->Add_State(CState_SimonManusP1_StingAttack::Create(m_pFsmCom, this, ATK_STING, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_Stamp::Create(m_pFsmCom, this, ATK_STAMP, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_SwingMultiple::Create(m_pFsmCom, this, ATK_SWINGMULTIPLE, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_ChasingSwing::Create(m_pFsmCom, this, ATK_CHASINGSWING, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_SwipMultiple_R::Create(m_pFsmCom, this, ATK_SWIPMULT_R, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_SwipMultiple_L::Create(m_pFsmCom, this, ATK_SWIPMULT_L, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_SwingDown_Swing_R::Create(m_pFsmCom, this, ATK_SWINGDOWN_R, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_SwingDown_Swing_L::Create(m_pFsmCom, this, ATK_SWINGDOWN_L, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_JumpToSwing::Create(m_pFsmCom, this, ATK_JUMPTOSWING, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_HighJumpFall::Create(m_pFsmCom, this, ATK_HIGHJUMPFALL, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_Charge_SwingDown::Create(m_pFsmCom, this, ATK_CHARGE_SWINGDOWN, &Desc));

	
	m_pFsmCom->Set_State(IDLE);
#pragma endregion

#pragma region Phase2_Fsm
	//2페이즈용 FSM

	/* FOR.Com_FSM */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		TEXT("Com_ExtraFSM"), reinterpret_cast<CComponent**>(&m_pExtraFsmCom))))
		return E_FAIL;

	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Idle::Create(m_pExtraFsmCom, this, IDLE, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Grogy::Create(m_pExtraFsmCom, this, GROGY, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_HitFatal::Create(m_pExtraFsmCom, this, HITFATAL, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Die::Create(m_pExtraFsmCom, this, DIE, &Desc));

	m_pExtraFsmCom->Add_State(CState_SimonManusP2_BrutalAttack::Create(m_pExtraFsmCom, this, ATKP2_BRUTALATTACK, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_HighJumpFall::Create(m_pExtraFsmCom, this, ATKP2_HIGHJUMPFALL, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_SpreadMagic::Create(m_pExtraFsmCom, this, ATKP2_SPREADMAGIC, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Sting::Create(m_pExtraFsmCom, this, ATKP2_STING, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_SummonHand::Create(m_pExtraFsmCom, this, ATKP2_SUMMONHAND, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_ThunderBall::Create(m_pExtraFsmCom, this, ATKP2_THUNDERBALL, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_ThunderCalling::Create(m_pExtraFsmCom, this, ATKP2_THUNDERCALLING, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Wave::Create(m_pExtraFsmCom, this, ATKP2_WAVE, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_SlideMagic::Create(m_pExtraFsmCom, this, ATKP2_SLIDEMAGIC, &Desc));
	
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_ChasingSwing::Create(m_pExtraFsmCom, this, ATKP2_CHASINGSWING, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_JumpToAttack::Create(m_pExtraFsmCom, this, ATKP2_JUMPTOATTACK, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_BackLightning::Create(m_pExtraFsmCom, this, ATKP2_BACKLIGHTNING, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Stamp::Create(m_pExtraFsmCom, this, ATKP2_STAMP, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_SwingDown_Swing::Create(m_pExtraFsmCom, this, ATKP2_SWINGDOWN_SWING, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_SwipMultiple::Create(m_pExtraFsmCom, this, ATKP2_SWIPMULTIPLE, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Route1::Create(m_pExtraFsmCom, this, ATKP2_ROUTE_1, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Route2::Create(m_pExtraFsmCom, this, ATKP2_ROUTE_2, &Desc));

	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Die_Talking::Create(m_pExtraFsmCom, this, DIE_TALKING, &Desc));

	//m_pExtraFsmCom->Set_State(IDLE);
#pragma endregion

#pragma region CutScene_Fsm
	/* FOR.Com_FSM */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		TEXT("Com_CutSCeneFSM"), reinterpret_cast<CComponent**>(&m_pCutSceneFsmCom))))
		return E_FAIL;

	m_pCutSceneFsmCom->Add_State(CState_SimonManus_CutScene_Meet::Create(m_pCutSceneFsmCom, this, STATE_MEET, &Desc));
	m_pCutSceneFsmCom->Add_State(CState_SimonManus_CutScene_Phase2::Create(m_pCutSceneFsmCom, this, STATE_P2, &Desc));
#pragma endregion
	return S_OK;
}

HRESULT CSimonManus::Ready_Weapon()
{
	CWeapon::MONSTER_WAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(66);	//Weapon_R 66

	WeaponDesc.pParentAtk = &m_eStat.fAtk;

	WeaponDesc.pMonster = this;

	m_pWeapon = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_SimonManus_Hammer"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return E_FAIL;

	m_pWeapon->Appear();

	return S_OK;
}

HRESULT CSimonManus::Ready_Effects()
{
	m_Effects.resize(EFFECT_END);

	const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	const _Matrix* pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(66);

	m_Effects[P1_CHARGEHAMMER] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Weapon_ChargeSwing"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT));

	m_Effects[P1_TRAIL] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_Swing"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[P1_CHARGEHAMMER] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Weapon_ChargeSwing"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[SWING_DRAG] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_Swing_Drag"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[SWING_DRAG_REVERSE] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_Swing_Drag"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f), _Vec3(0.f, 0.f, 180.f));

	m_Effects[WEAPON_PARTICLE] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Weapon_Follow"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	pSocketBoneMatrix = m_pExtraModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pExtraModelCom->Get_UFBIndices(UFB_HAND_LEFT));

	m_Effects[P2_SLIDEMAGIC] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_SlideMagic"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[P2_JUMPMAGIC] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_JumpMagic"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));


	pSocketBoneMatrix = m_pExtraModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pExtraModelCom->Get_UFBIndices(UFB_HAND_RIGHT));

	m_Effects[P2_SH_CHARGE] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_SummonHand_Charge"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[P2_SH_EXPLOSION] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_SummonHand_Explosion"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[P2_TRAIL] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_Swing_2P"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	pSocketBoneMatrix = m_pExtraModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pExtraModelCom->Get_UFBIndices(UFB_HAND_LEFT));

	m_Effects[P2_WAVE_TRAIL] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_Wave"), pParetnMatrix,
		pSocketBoneMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

#pragma region 컷신용 이펙트들
	m_Effects[CUTSCENE_P2_MAP] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("Map_SimonManus_2P"), nullptr,
		nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[CUTSCENE_P2_ARM_PARTICLE] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_CutScene_2P_Open"), pParetnMatrix,
		nullptr, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

#pragma endregion


	for (auto& pEffect : m_Effects)
	{
		pEffect->Set_Loop(false);
		pEffect->Set_Dead(true);
	}
	m_Effects[WEAPON_PARTICLE]->Set_Loop(true);

	m_DissolveEffects.resize(DISSOLVE_END);

	// 고준호
	CSimonManus_2P_Aura::DISSOLVE_OBJECT_DESC AuraDesc = {};
	AuraDesc.fRotationPerSec = XMConvertToRadians(90.f);
	AuraDesc.fSpeedPerSec = 1.f;
	AuraDesc.iLevelIndex = LEVEL_GAMEPLAY;
	AuraDesc.pTarget_TransformCom = m_pTransformCom;
	AuraDesc.pTarget_ModelCom = m_pExtraModelCom;
	AuraDesc.pCutSceneModelCom = m_pCutSceneModelCom[MODEL_PHASE2];
	AuraDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Dissolve_SimonManusP2_Aura");

	m_DissolveEffects[DISSOLVE_AURA] = static_cast<CSimonManus_2P_Aura*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Dissolve_SimonManus_2P_Aura"), &AuraDesc));

	CDissolve_SimonManus_Dead::DISSOLVE_OBJECT_DESC DissolveDesc = {};
	DissolveDesc.fRotationPerSec = 90.f;
	DissolveDesc.fSpeedPerSec = 1.f;
	DissolveDesc.iLevelIndex = LEVEL_GAMEPLAY;
	DissolveDesc.pTarget_ModelCom = m_pExtraModelCom;
	DissolveDesc.pTarget_TransformCom = m_pTransformCom;
	DissolveDesc.pDissolveTextureCom = m_pDissloveTexture;
	DissolveDesc.pThreshold = &m_fDissloveRatio;
	DissolveDesc.vTextureSize = _float2(2048.f, 2048.f);
	DissolveDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Dissolve_SimonManus_Dead");

	m_DissolveEffects[DISSOLVE_DEAD] = static_cast<CDissolve_SimonManus_Dead*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Dissolve_SimonManus_Dead"), &DissolveDesc));

	CDissolve_Effect::DISSOLVE_EFFECT_DESC PowerAttackDesc = {};
	PowerAttackDesc.fRotationPerSec = XMConvertToRadians(90.f);
	PowerAttackDesc.fSpeedPerSec = 1.f;
	PowerAttackDesc.iLevelIndex = LEVEL_GAMEPLAY;
	PowerAttackDesc.pTarget_ModelCom = m_pP1ModelCom;
	PowerAttackDesc.pTarget_TransformCom = m_pTransformCom;
	PowerAttackDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Dissolve_SimonManusP1_PowerAttack");
	m_DissolveEffects[DISSOLVE_POWERATTACK_P1] = static_cast<CDissolve_PowerAttack*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Dissolve_PowerAttack"), &PowerAttackDesc));
	
	PowerAttackDesc.pTarget_ModelCom = m_pExtraModelCom;
	PowerAttackDesc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Dissolve_SimonManusP2_PowerAttack");
	m_DissolveEffects[DISSOLVE_POWERATTACK_P2] = static_cast<CDissolve_PowerAttack*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Dissolve_PowerAttack"), &PowerAttackDesc));


	return S_OK;
}

void CSimonManus::Update_Collider()
{
	_float4x4 UpdateMat{};

	_Matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
	XMStoreFloat4x4(&UpdateMat							//척추2(상하체 분리부)
		, *(m_pColliderBindMatrix[CT_UPPERBODY]) * WorldMat);
	m_pColliderCom->Update(&UpdateMat);

	XMStoreFloat4x4(&UpdateMat							//골반()
		, *(m_pColliderBindMatrix[CT_LOWERBODY]) * WorldMat);
	m_EXCollider[LOWERBODY]->Update(&UpdateMat);

	//변경되는 다리 뼈 부분을 체인지 페이즈 할때 같이 바꿔주도록
	XMStoreFloat4x4(&UpdateMat							//왼 종아리()
		, *(m_pColliderBindMatrix[CT_LEG_LEFT]) * WorldMat);
	m_EXCollider[LEG_LEFT]->Update(&UpdateMat);

	XMStoreFloat4x4(&UpdateMat							//오른 종아리()
		, *(m_pColliderBindMatrix[CT_LEG_RIGHT]) * WorldMat);
	m_EXCollider[LEG_RIGHT]->Update(&UpdateMat);
}

void CSimonManus::ChangePhase()
{
	//모델, fsm 정리 후 엑스트라에 있던 컴포넌트들을 소유하도록, 그 후 초기화 작업까지
	if (m_pExtraModelCom == nullptr || m_pExtraFsmCom == nullptr)
	{
		return;
	}

	m_pFsmCom->Release_States();
	
	//Safe_Release(m_pModelCom);
	Safe_Release(m_pFsmCom);

	m_pModelCom = m_pExtraModelCom;
	m_pFsmCom = m_pExtraFsmCom;

	m_pExtraModelCom = nullptr;
	m_pExtraFsmCom = nullptr;

	m_pModelCom->SetUp_Animation(8, true);//P2 Idle
	m_pFsmCom->Set_State(IDLE);

	m_pModelCom->Play_Animation(0);		//업데이트만 한번


	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};

	ColliderDesc.vExtents = _float3(0.6f, 0.3f, 0.3f);
	ColliderDesc.vCenter = _float3(0.4f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	(m_EXCollider[LEG_LEFT])->Change_BoundingDesc(&ColliderDesc);
	(m_EXCollider[LEG_RIGHT])->Change_BoundingDesc(&ColliderDesc);
	 
	m_pColliderBindMatrix[CT_LEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(173);
	m_pColliderBindMatrix[CT_LEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(187);

	ColliderDesc.vExtents = _float3(0.8f, 0.7f, 0.7f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.2f);
	ColliderDesc.vAngles = _float3(0.f, -0.3f, 0.f);

	(m_EXCollider[LOWERBODY])->Change_BoundingDesc(&ColliderDesc);

	m_pColliderBindMatrix[CT_LOWERBODY] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(6);

	ColliderDesc.vExtents = _float3(1.5f, 1.5f, 1.5f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vAngles = _float3(0.2f, -0.2f, -0.1f);

	(m_pColliderCom)->Change_BoundingDesc(&ColliderDesc);

	m_pColliderBindMatrix[CT_UPPERBODY] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(72);

	m_pWeapon->ChangeSocketMatrix(m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(46));

	CEffect_Container::EFFECT_DESC Desc;
	Desc.pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	Desc.pSocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT));
	m_Effects[WEAPON_PARTICLE]->Set_EffectDesc(Desc);
	m_Effects[SWING_DRAG]->Set_EffectDesc(Desc);
	
	Desc.vRotate = _Vec3{0.f, 0.f, 180.f};
	m_Effects[SWING_DRAG_REVERSE]->Set_EffectDesc(Desc);

	Desc.pSocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_WEAPON));
	m_Effects[P1_CHARGEHAMMER]->Set_EffectDesc(Desc);

	m_eStat.fHp = 4000.f;
	m_eStat.fMaxHp = 4000.f;
	m_eStat.fAtk = 270.f;
	//m_eStat.fDefence = 8.f;

	m_eStat.fMaxGrogyPoint = 390.f;
	m_eStat.fGrogyPoint = 0.f;

	const _Matrix* pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("Bip002-Spine");

	m_Effects[CUTSCENE_P2_ARM_PARTICLE]->Set_Matrices(pSocketBoneMatrix);
	On_Aura(true, false);

	m_isDead = false;
	m_isChanged = true;
}

CSimonManus* CSimonManus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSimonManus* pInstance = new CSimonManus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CSimonManus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CSimonManus::Clone(void* pArg)
{
	CSimonManus* pInstance = new CSimonManus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CSimonManus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSimonManus::Free()
{
	for (_uint i = 0; i < COLLTYPE_END; ++i)
	{
		Safe_Release(m_EXCollider[i]);
	}
	Safe_Release(m_pWeapon);

	Safe_Release(m_pDissloveTexture);

	if (m_pModelCom != m_pP1ModelCom)
		Safe_Release(m_pP1ModelCom);
	if (m_pModelCom != m_pExtraModelCom)
		Safe_Release(m_pExtraModelCom);
	if (m_pModelCom != m_pCutSceneModelCom[0])
		Safe_Release(m_pCutSceneModelCom[0]);
	if (m_pModelCom != m_pCutSceneModelCom[1])
		Safe_Release(m_pCutSceneModelCom[1]);

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