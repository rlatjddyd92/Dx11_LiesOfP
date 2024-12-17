#include "stdafx.h"
#include "..\Public\SimonManus.h"

#include "GameInstance.h"
#include "Player.h"
#include "Fsm.h"

// 24-11-26 김성용
// 게임인터페이스 접근 코드 
// 정식 코드  
#include "GameInterface_Controller.h"

#include "Effect_Manager.h"
#include "Effect_Container.h"
//전부 수정하기

#include "Weapon.h"
#include "CutScene.h"

#pragma region Phase1
#include "State_SimonManusP1_Idle.h"
#include "State_SimonManusP1_Die.h"
#include "State_SimonManusP1_Grogy.h"
#include "State_SimonManusP1_HitFatal.h"
#include "State_SimonManusP1_Walk.h"
#include "State_SimonManusP1_Run.h"

#include "State_SimonManusP1_StingAttack.h"
#include "State_SimonManusP1_Stamp.h"
#include "State_SimonManusP1_AvoidSwing.h"
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
#include "State_SimonManusP2_Walk.h"
#include "State_SimonManusP2_Run.h"

#include "State_SimonManusP2_ChasingSwing.h"
#include "State_SimonManusP2_LightningToWave.h"
#include "State_SimonManusP2_SwingDown_Swing.h"
#include "State_SimonManusP2_Stamp.h"
#include "State_SimonManusP2_SwipMultiple.h"
#include "State_SimonManusP2_JumpToAttack.h"
#include "State_SimonManusP2_Route0.h"
#include "State_SimonManusP2_Route1.h"
#include "State_SimonManusP2_Route2.h"

#include "State_SimonManusP2_AvoidSwing.h"
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

	m_eStat.strName = TEXT("시몬 마누스");

	m_eStat.fHp = 200.f;
	m_eStat.fMaxHp = 200.f;
	m_eStat.fAtk = 10.f;
	m_eStat.fDefence = 5.f;
	m_eStat.fStemina = 100.f;
	m_eStat.bWeakness = false;
	m_eStat.fGrogyPoint = 0.f;
	m_eStat.fMaxGrogyPoint = 50.f;

	m_eHitType = HIT_CARCASS;

	m_pWeapon->DeActive_Collider();

	m_pTransformCom->Rotation(0.f, 50.f, 0.f);

	//m_pModelCom->SetUp_NextAnimation(12);
	// 24-11-26 김성용
	// 몬스터 직교 UI 접근 코드 
	// 정식 코드  
	GET_GAMEINTERFACE->Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_SIMON, this);

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);

	Start_CutScene(CUTSCENE_MEET);

	return S_OK;
}

void CSimonManus::Priority_Update(_float fTimeDelta)
{

	__super::Set_UpTargetPos();
	m_pWeapon->Priority_Update(fTimeDelta);

	if (!m_bDieState && m_eStat.fHp <= 0.f)
	{
		m_bDieState = true;

		if(m_isChanged)
			dynamic_cast<CCutScene*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), BOSS2_DEFEAT))->Start_Play();
		else
			dynamic_cast<CCutScene*>(m_pGameInstance->Find_Object(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), BOSS2_PHASE2))->Start_Play();

		if (m_isChanged)
		{
			GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
		}

	}
	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Priority_Update(fTimeDelta);
	}
}

void CSimonManus::Update(_float fTimeDelta)
{
	if (KEY_TAP(KEY::B))
	{
		ChangePhase();
	}
	//if (KEY_TAP(KEY::Z))
	//{
	//	m_Effects[P2_JUMPMAGIC]->Set_Loop(true);
	//}

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

	Update_Collider();

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

#ifdef _DEBUG
	m_pColliderCom->Render();
	m_EXCollider[LEG_LEFT]->Render();
	m_EXCollider[LEG_RIGHT]->Render();
	m_EXCollider[LOWERBODY]->Render();
#endif

	return S_OK;
}

void CSimonManus::Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex, _uint iHitType, _uint iAtkStrength)
{
	m_pWeapon->Active_Collider(fDamageRatio, iCollIndex, iHitType, iAtkStrength);
}

void CSimonManus::DeActive_CurretnWeaponCollider(_uint iCollIndex)
{
	m_pWeapon->DeActive_Collider();
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

	switch (iCutSceneNum)
	{
	case CUTSCENE_MEET :
		m_pModelCom = m_pCutSceneModelCom[MODEL_PHASE1];
		pNewSocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("Bn_Weapon_R");
		m_pWeapon->ChangeSocketMatrix(pNewSocketMatrix);
		m_pCutSceneFsmCom->Set_State(STATE_MEET);
		Deactiave_Weapon();
		break;
	case CUTSCENE_P2:
		m_pModelCom = m_pCutSceneModelCom[MODEL_PHASE1];
		pNewSocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("Bn_Weapon_R");
		m_pWeapon->ChangeSocketMatrix(pNewSocketMatrix);
		m_pCutSceneFsmCom->Change_State(STATE_P2);
		m_isPlayAnimation = true;
		Deactiave_Weapon();
		m_pRigidBodyCom->Set_GloblePose(_Vec3(0.f, 0.f, 0.f));
		m_pTransformCom->Rotation(0.f, 50.f, 0.f);
		break;
	case CUTSCENE_DIE:
		m_pModelCom = m_pCutSceneModelCom[MODEL_PHASE2];
		break;
	}

	m_isCutScene = true;
}

void CSimonManus::End_CutScene(_uint iCutSceneNum)
{
	if (m_pCutSceneFsmCom->Get_CurrentState() == STATE_MEET)
	{
		m_pModelCom = m_pP1ModelCom;

		const _Matrix* pNewSocketMatrix = { nullptr };
		pNewSocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("Bn_Weapon_R");
		m_pWeapon->ChangeSocketMatrix(pNewSocketMatrix);
	}
	else if (m_pCutSceneFsmCom->Get_CurrentState() == STATE_P2)
	{
		ChangePhase();
	}
	m_isCutScene = false;
}

void CSimonManus::Change_Model(_uint iModelNum)	// 컷신 2페이즈로 바꾸는 용도
{
	m_pModelCom = m_pCutSceneModelCom[1];
	Deactiave_Weapon();
}

HRESULT CSimonManus::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

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
	ColliderDesc.vExtents = _float3(0.3f, 0.6f, 0.3f);
	ColliderDesc.vCenter = _float3(-0.1f, -0.4f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, -0.45f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_LL"), reinterpret_cast<CComponent**>(&m_EXCollider[LEG_LEFT]), &ColliderDesc)))
		return E_FAIL;

	//R
	ColliderDesc.vExtents = _float3(0.3f, 0.6f, 0.3f);
	ColliderDesc.vCenter = _float3(-0.1f, -0.4f, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, -0.45f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_RL"), reinterpret_cast<CComponent**>(&m_EXCollider[LEG_RIGHT]), &ColliderDesc)))
		return E_FAIL;

	//LowerBody
	ColliderDesc.vExtents = _float3(0.6f, 1.f, 0.8f);
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
	CapsuleDesc.fRadius = 0.45f;
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
	m_pFsmCom->Add_State(CState_SimonManusP1_Walk::Create(m_pFsmCom, this, WALK, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_Run::Create(m_pFsmCom, this, RUN, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_Grogy::Create(m_pFsmCom, this, GROGY, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_HitFatal::Create(m_pFsmCom, this, HITFATAL, &Desc));
	m_pFsmCom->Add_State(CState_SimonManusP1_Die::Create(m_pFsmCom, this, DIE, &Desc));

	m_pFsmCom->Add_State(CState_SimonManusP1_AvoidSwing::Create(m_pFsmCom, this, ATK_AVOIDSWING, &Desc));
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
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Walk::Create(m_pExtraFsmCom, this, WALK, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Run::Create(m_pExtraFsmCom, this, RUN, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Grogy::Create(m_pExtraFsmCom, this, GROGY, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_HitFatal::Create(m_pExtraFsmCom, this, HITFATAL, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Die::Create(m_pExtraFsmCom, this, DIE, &Desc));

	m_pExtraFsmCom->Add_State(CState_SimonManusP2_AvoidSwing::Create(m_pExtraFsmCom, this, ATKP2_AVOIDSWING, &Desc));
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
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_LightningToWave::Create(m_pExtraFsmCom, this, ATKP2_LIGHTNINGTOWAVE, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Stamp::Create(m_pExtraFsmCom, this, ATKP2_STAMP, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_SwingDown_Swing::Create(m_pExtraFsmCom, this, ATKP2_SWINGDOWN_SWING, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_SwipMultiple::Create(m_pExtraFsmCom, this, ATKP2_SWIPMULTIPLE, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Route0::Create(m_pExtraFsmCom, this, ATKP2_ROUTE_0, &Desc));
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
	CWeapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(66);	//Weapon_R 66

	WeaponDesc.pParentAtk = &m_eStat.fAtk;

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
	const _Matrix* pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT));

	m_Effects[P1_TRAIL] = CEffect_Manager::Get_Instance()->Clone_Effect(TEXT("SimonManus_Attack_Swing"), pParetnMatrix,
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


	for (auto& pEffect : m_Effects)
	{
		pEffect->Set_Loop(false);
		pEffect->Set_Dead(true);
	}


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
	
	Safe_Release(m_pModelCom);
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
	m_Effects[P1_TRAIL]->Set_EffectDesc(Desc);

	m_eStat.fHp = 200.f;
	m_eStat.fMaxHp = 200.f;
	m_eStat.fAtk = 15.f;
	m_eStat.fDefence = 8.f;
	m_eStat.fStemina = 100.f;
	m_eStat.fMaxGrogyPoint = 50.f;
	m_eStat.fGrogyPoint = 0.f;

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
	Safe_Release(m_pP1ModelCom);
	Safe_Release(m_pExtraModelCom);
	Safe_Release(m_pCutSceneModelCom[0]);
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

	__super::Free();

}