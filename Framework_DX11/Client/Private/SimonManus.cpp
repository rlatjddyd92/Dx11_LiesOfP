#include "stdafx.h"
#include "..\Public\SimonManus.h"

#include "GameInstance.h"
#include "Player.h"
#include "Fsm.h"

//전부 수정하기

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
#pragma endregion

#include "Weapon.h"

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

	m_pModelCom->SetUp_Animation(rand() % 20, true);

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	if (FAILED(Ready_Weapon()))
		return E_FAIL;

	m_strObjectTag = TEXT("Monster");
	m_pColliderCom->Set_Owner(this);

	for (_uint i = 0; i < EXCOLLIDER::COLLTYPE_END; ++i)
	{
		m_EXCollider[i]->Set_Owner(this);
	}

	m_fHp = 100.f;
	m_fAtk = 10.f;
	m_fDefence = 5.f;
	m_fStemina = 100.f;

	m_pWeapon->DeActive_Collider();

	return S_OK;
}

void CSimonManus::Priority_Update(_float fTimeDelta)
{

	__super::Set_UpTargetPos();
	m_pWeapon->Priority_Update(fTimeDelta);
}

void CSimonManus::Update(_float fTimeDelta)
{
	m_vCurRootMove = XMVector3TransformNormal(m_pModelCom->Play_Animation(fTimeDelta), m_pTransformCom->Get_WorldMatrix());

	m_pRigidBodyCom->Set_Velocity(m_vCurRootMove / fTimeDelta);

	m_pFsmCom->Update(fTimeDelta);

	if (KEY_TAP(KEY::B))
	{
		ChangePhase();
	}

	_float4x4 UpdateMat{};
	XMStoreFloat4x4(&UpdateMat							//척추2(상하체 분리부)
		, m_pModelCom->Get_BoneCombindTransformationMatrix(6) * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
	m_pColliderCom->Update(&UpdateMat);

	XMStoreFloat4x4(&UpdateMat							//골반()
		, m_pModelCom->Get_BoneCombindTransformationMatrix(5) * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
	m_EXCollider[LOWERBODY]->Update(&UpdateMat);
	if (!m_isChanged)
	{
		XMStoreFloat4x4(&UpdateMat							//왼 종아리()
			, m_pModelCom->Get_BoneCombindTransformationMatrix(112) * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
		m_EXCollider[LEG_LEFT]->Update(&UpdateMat);

		XMStoreFloat4x4(&UpdateMat							//오른 종아리()
			, m_pModelCom->Get_BoneCombindTransformationMatrix(126) * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
		m_EXCollider[LEG_RIGHT]->Update(&UpdateMat);

	}
	else
	{
		XMStoreFloat4x4(&UpdateMat							//왼 종아리()
			, m_pModelCom->Get_BoneCombindTransformationMatrix(173) * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
		m_EXCollider[LEG_LEFT]->Update(&UpdateMat);

		XMStoreFloat4x4(&UpdateMat							//오른 종아리()
			, m_pModelCom->Get_BoneCombindTransformationMatrix(187) * XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
		m_EXCollider[LEG_RIGHT]->Update(&UpdateMat);

	}

	m_pWeapon->Update(fTimeDelta);
}

void CSimonManus::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pRigidBodyCom->Update(fTimeDelta);

	m_pWeapon->Late_Update(fTimeDelta);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	for (_uint i = 0; i < EXCOLLIDER::COLLTYPE_END; ++i)
	{
		m_pGameInstance->Add_ColliderList(m_EXCollider[i]);
	}
}

HRESULT CSimonManus::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pWeapon->Render()))
		return E_FAIL;

	m_pColliderCom->Render();
	m_EXCollider[LEG_LEFT]->Render();
	m_EXCollider[LEG_RIGHT]->Render();
	m_EXCollider[LOWERBODY]->Render();

	return S_OK;
}

void CSimonManus::Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex)
{
	m_pWeapon->Active_Collider(fDamageRatio);
}

void CSimonManus::DeActive_CurretnWeaponCollider(_uint iCollIndex)
{
	m_pWeapon->DeActive_Collider();
}

HRESULT CSimonManus::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManusP1"),
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

	for (_uint i = 0; i < TYPE_END; ++i)
		m_EXCollider[i]->Set_Owner(this);
		

	// 항상 마지막에 생성하기
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc{};
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerTransform = m_pTransformCom;
	RigidBodyDesc.pOwnerNavigation = m_pNavigationCom;

	RigidBodyDesc.pOwner = this;
	RigidBodyDesc.fStaticFriction = 0.f;
	RigidBodyDesc.fDynamicFriction = 0.f;
	RigidBodyDesc.fRestituion = 0.f;

	physX::GeometryCapsule CapsuleDesc;
	CapsuleDesc.fHeight = 2.5f;
	CapsuleDesc.fRadius = 0.45f;
	RigidBodyDesc.pGeometry = &CapsuleDesc;

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
	
	Desc.pIsEndAnim = &m_bEndAnim;
	Desc.pIsResetRootMove =&m_bResetRootMove;
	Desc.pRootMoveCtr = &m_bRootMoveCtr;
	//


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

	m_pExtraFsmCom->Add_State(CState_SimonManusP2_ChasingSwing::Create(m_pExtraFsmCom, this, ATKP2_CHASINGSWING, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_JumpToAttack::Create(m_pExtraFsmCom, this, ATKP2_JUMPTOATTACK, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_LightningToWave::Create(m_pExtraFsmCom, this, ATKP2_LIGHTNINGTOWAVE, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Stamp::Create(m_pExtraFsmCom, this, ATKP2_STAMP, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_SwingDown_Swing::Create(m_pExtraFsmCom, this, ATKP2_SWINGDOWN_SWING, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_SwipMultiple::Create(m_pExtraFsmCom, this, ATKP2_SWIPMULTIPLE, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Route0::Create(m_pExtraFsmCom, this, ATKP2_ROUTE_0, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Route1::Create(m_pExtraFsmCom, this, ATKP2_ROUTE_1, &Desc));
	m_pExtraFsmCom->Add_State(CState_SimonManusP2_Route2::Create(m_pExtraFsmCom, this, ATKP2_ROUTE_2, &Desc));

	//m_pExtraFsmCom->Set_State(IDLE);
#pragma endregion

	return S_OK;
}

HRESULT CSimonManus::Ready_Weapon()
{
	CWeapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(66);	//Weapon_R


	m_pWeapon = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_SimonManus_Hammer"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return E_FAIL;

	m_pWeapon->Appear();

	return S_OK;
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

	m_pWeapon->ChangeSocketMatrix(m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(46));

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
	Safe_Release(m_pExtraModelCom);

	if (m_pExtraFsmCom != nullptr)
	{
		m_pExtraFsmCom->Release_States();
	}
	Safe_Release(m_pExtraFsmCom);
	__super::Free();

}