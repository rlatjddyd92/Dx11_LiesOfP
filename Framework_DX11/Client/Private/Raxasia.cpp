#include "stdafx.h"
#include "..\Public\Raxasia.h"

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

#pragma region Phase1
#pragma endregion

#pragma region Phase2
#pragma endregion

#include "Weapon.h"

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

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->LookAt(_vector{ 0, 0, -1, 0 });

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->SetUp_Animation(8, true);

	//if (FAILED(Ready_FSM()))
	//	return E_FAIL;

	if (FAILED(Ready_Weapon()))
		return E_FAIL;

	//if (FAILED(Ready_Effects()))
	//	return E_FAIL;

	m_strObjectTag = TEXT("Monster");
	m_pColliderCom->Set_Owner(this);

	//for (_uint i = 0; i < EXCOLLIDER::COLLTYPE_END; ++i)
	//{
	//	m_EXCollider[i]->Set_Owner(this);
	//}

	m_eStat.strName = TEXT("락사시아");

	m_eStat.fHp = 200.f;
	m_eStat.fMaxHp = 200.f;
	m_eStat.fAtk = 10.f;
	m_eStat.fDefence = 5.f;
	m_eStat.fStemina = 100.f;
	m_eStat.bWeakness = false;
	m_eStat.fGrogyPoint = 0.f;
	m_eStat.fMaxGrogyPoint = 50.f;

	m_eHitType = HIT_METAL;

	m_pWeapon->DeActive_Collider();
	// 24-12-11 김성용
	// 몬스터 직교 UI 접근 코드 
	// 정식 코드  
	GET_GAMEINTERFACE->Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE::ORTHO_BOSS_RAXASIA, this);

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);

	return S_OK;
}

void CRaxasia::Priority_Update(_float fTimeDelta)
{

	__super::Set_UpTargetPos();
	m_pWeapon->Priority_Update(fTimeDelta);

	if (!m_bDieState && m_eStat.fHp <= 0.f)
	{
		m_bDieState = true;
		m_pFsmCom->Set_State(DIE);
		if (m_isChanged)
		{
			// 24-12-11 김성용
			// 몬스터 직교 UI OFF 코드 
			// 정식 코드  
			GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
		}
	}
	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
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

	m_vCurRootMove = XMVector3TransformNormal(m_pModelCom->Play_Animation(fTimeDelta), m_pTransformCom->Get_WorldMatrix());

	m_pRigidBodyCom->Set_Velocity(m_vCurRootMove / fTimeDelta);

	//m_pFsmCom->Update(fTimeDelta);

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

void CRaxasia::Late_Update(_float fTimeDelta)
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

	//for (_uint i = 0; i < EXCOLLIDER::COLLTYPE_END; ++i)
	//{
	//	m_pGameInstance->Add_ColliderList(m_EXCollider[i]);
	//}
}

HRESULT CRaxasia::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pWeapon->Render()))
		return E_FAIL;

#ifdef _DEBUG
	m_pColliderCom->Render();
	//m_EXCollider[LEG_LEFT]->Render();
	//m_EXCollider[LEG_RIGHT]->Render();
	//m_EXCollider[LOWERBODY]->Render();
#endif

	return S_OK;
}

void CRaxasia::Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex, _uint iHitType, _uint iAtkStrength)
{
	m_pWeapon->Active_Collider(fDamageRatio, iCollIndex, iHitType, iAtkStrength);
}

void CRaxasia::DeActive_CurretnWeaponCollider(_uint iCollIndex)
{
	m_pWeapon->DeActive_Collider();
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

HRESULT CRaxasia::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RaxasiaP1"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_ExtraModel */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManusP2"),
		TEXT("Com_ExtraModel"), reinterpret_cast<CComponent**>(&m_pExtraModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */		//Body
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(1.3f, 1.3f, 1.7f);
	ColliderDesc.vCenter = _float3(1.4f, 0.3f, 0.35f);
	ColliderDesc.vAngles = _float3(-0.3f, 0.f, 0.2f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	//for (_uint i = 0; i < COLLTYPE_END; ++i)
	//	m_EXCollider[i]->Set_Owner(this);


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

HRESULT CRaxasia::Ready_FSM()
{
	if (FAILED(__super::Ready_FSM()))
		return E_FAIL;

	FSM_INIT_DESC Desc{};



#pragma region Phase1_Fsm

	//m_pFsmCom->Set_State(IDLE);
#pragma endregion

#pragma region Phase2_Fsm
	//2페이즈용 FSM

	/* FOR.Com_FSM */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
	//	TEXT("Com_ExtraFSM"), reinterpret_cast<CComponent**>(&m_pExtraFsmCom))))
	//	return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CRaxasia::Ready_Weapon()
{
	CWeapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(62);	//Weapon_R

	WeaponDesc.pParentAtk = &m_eStat.fAtk;

	m_pWeapon = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon__Raxasia_P1_Sword"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return E_FAIL;

	m_pWeapon->Appear();

	return S_OK;
}

HRESULT CRaxasia::Ready_Effects()
{
	m_Effects.resize(EFFECT_END);

	const _Matrix* pParetnMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	const _Matrix* pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(m_pModelCom->Get_UFBIndices(UFB_HAND_RIGHT));


	//for (auto& pEffect : m_Effects)
	//{
	//	pEffect->Set_Loop(false);
	//	pEffect->Set_Dead(true);
	//}


	return S_OK;
}

void CRaxasia::Update_Collider()
{
	_float4x4 UpdateMat{};

	_Matrix WorldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
	//XMStoreFloat4x4(&UpdateMat							//척추2(상하체 분리부)
	//	, *(m_pColliderBindMatrix[CT_UPPERBODY]) * WorldMat);
	//m_pColliderCom->Update(&UpdateMat);
	//
	//XMStoreFloat4x4(&UpdateMat							//골반()
	//	, *(m_pColliderBindMatrix[CT_LOWERBODY]) * WorldMat);
	//m_EXCollider[LOWERBODY]->Update(&UpdateMat);
	//
	////변경되는 다리 뼈 부분을 체인지 페이즈 할때 같이 바꿔주도록
	//XMStoreFloat4x4(&UpdateMat							//왼 종아리()
	//	, *(m_pColliderBindMatrix[CT_LEG_LEFT]) * WorldMat);
	//m_EXCollider[LEG_LEFT]->Update(&UpdateMat);
	//
	//XMStoreFloat4x4(&UpdateMat							//오른 종아리()
	//	, *(m_pColliderBindMatrix[CT_LEG_RIGHT]) * WorldMat);
	//m_EXCollider[LEG_RIGHT]->Update(&UpdateMat);
}

void CRaxasia::ChangePhase()
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

	//m_pColliderBindMatrix[CT_LEG_LEFT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(173);
	//m_pColliderBindMatrix[CT_LEG_RIGHT] = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(187);

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
	//m_Effects[P1_TRAIL]->Set_EffectDesc(Desc);

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
	Safe_Release(m_pWeapon);
	Safe_Release(m_pExtraModelCom);

	for (auto& pEffect : m_Effects)
	{
		pEffect->Set_Cloned(false);
		Safe_Release(pEffect);
	}
	m_Effects.clear();

	if (m_pExtraFsmCom != nullptr)
	{
		m_pExtraFsmCom->Release_States();
	}
	Safe_Release(m_pExtraFsmCom);
	__super::Free();

}