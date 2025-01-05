#include "stdafx.h"
#include "Monster_Training02.h"

#include "GameInstance.h"
#include "Player.h"
#include "Fsm.h"

#include "State_Training02_Idle.h"
#include "State_Training01_Attack.h"
#include "State_Training_Grogy.h"
#include "State_Training_HitFatal.h"

#include "GameInterface_Controller.h"


CMonster_Training02::CMonster_Training02(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CMonster_Training02::CMonster_Training02(const CMonster_Training02& Prototype)
	: CMonster{ Prototype }
{
}

HRESULT CMonster_Training02::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Training02::Initialize(void* pArg)
{
	OBJECT_DEFAULT_DESC* pDefaultDesc = static_cast<OBJECT_DEFAULT_DESC*>(pArg);

	CGameObject::GAMEOBJECT_DESC		Desc{};
	Desc.fSpeedPerSec = 1.5f;
	Desc.fRotationPerSec = 30.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	m_strObjectTag = TEXT("Monster");

	m_pNavigationCom->Move_to_Cell(m_pRigidBodyCom, 1315); 
	m_iInitRoomNum = m_pNavigationCom->Get_Cell_AreaNum(1315);

	m_pTransformCom->Rotation(_Vec4(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	m_vRimLightColor = { 0.f, 0.f, 0.f, 0.f };

	m_eStat.fHp = 3000.f;
	m_eStat.fMaxHp = 3000.f;
	m_eStat.fAtk = 4.f;
	//m_eStat.fDefence = 3.f;

	m_eStat.bWeakness = false;

	m_eStat.fGrogyPoint = 0.f;
	m_eStat.fMaxGrogyPoint = 5.f;

	m_vCenterOffset = _Vec3(0.f, 1.5f, 0.f);

	GET_GAMEINTERFACE->Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE::ORTHO_TRAINIG_MONSTER_NORMAL, this);

	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
	return S_OK;
}

void CMonster_Training02::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);

	if (m_eStat.fHp <= 0.f)
	{
		m_eStat.fHp = m_eStat.fMaxHp;
	}
}

void CMonster_Training02::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_Player_AreaNum() == m_iInitRoomNum)
		m_vCurRootMove = XMVector3TransformNormal(m_pModelCom->Play_Animation(fTimeDelta), m_pTransformCom->Get_WorldMatrix());
	else
		m_vCurRootMove = _Vec3(0.f, 0.f, 0.f);

	m_pRigidBodyCom->Set_Velocity(m_vCurRootMove / fTimeDelta);

	m_pFsmCom->Update(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
}

void CMonster_Training02::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_Player_AreaNum() == m_iInitRoomNum)
	{
		if (m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 50.f))
		{
			__super::Late_Update(fTimeDelta);

			m_pRigidBodyCom->Update(fTimeDelta);
			m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

			//m_pColliderObject->Late_Update(fTimeDelta);

			m_pGameInstance->Add_ColliderList(m_pColliderCom);
		}
	}
}

HRESULT CMonster_Training02::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	m_pColliderCom->Render();

#endif
	return S_OK;
}

_bool CMonster_Training02::Calc_DamageGain(_float fAtkDmg, _Vec3 vHitPos, _uint iHitType, _uint iAttackStrength, CGameObject* pAttacker)
{
	if (m_bDieState || m_isInvicible)
	{
		return false;
	}

	if (iAttackStrength == ATTACK_STRENGTH::ATK_STRONG)
	{
		if (m_eStat.bWeakness)
		{
			if (iAttackStrength >= ATTACK_STRENGTH::ATK_NORMAL)
			{
				m_eStat.bWeakness = false;
				m_eStat.bFatalAttack = true;

				m_pFsmCom->Change_State(GROGY);
				return true;
			}
		}
	}

	if (iAttackStrength == ATTACK_STRENGTH::ATK_LAST)
	{
		m_bFatalAttacked = true;
		m_eStat.bFatalAttack = false;
	}

	return true;
}

HRESULT CMonster_Training02::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Training02"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */		//Body
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.4f, 0.42f, 0.37f);
	ColliderDesc.vCenter = _float3(0.f, 1.4f, 0.1f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	// 항상 마지막에 생성하기
	CRigidBody::RIGIDBODY_DESC RigidBodyDesc{};
	RigidBodyDesc.isStatic = false;
	RigidBodyDesc.isGravity = false;
	RigidBodyDesc.pOwnerTransform = m_pTransformCom;
	RigidBodyDesc.pOwnerNavigation = m_pNavigationCom;
	RigidBodyDesc.isCapsule = true;

	RigidBodyDesc.pOwner = this;
	RigidBodyDesc.fStaticFriction = 1.0f;
	RigidBodyDesc.fDynamicFriction = 0.0f;
	RigidBodyDesc.fRestituion = 0.0f;

	physX::GeometryCapsule CapsuleDesc;
	CapsuleDesc.fHeight = 1.4f;
	CapsuleDesc.fRadius = 0.5f;
	RigidBodyDesc.pGeometry = &CapsuleDesc;
	RigidBodyDesc.PxLockFlags = PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
		PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &RigidBodyDesc)))
		return E_FAIL;
	m_pRigidBodyCom->Set_Mass(10000.f);

	return S_OK;
}

HRESULT CMonster_Training02::Ready_FSM()
{
	if (FAILED(__super::Ready_FSM()))
		return E_FAIL;

	FSM_INIT_DESC Desc{};

	m_pFsmCom->Add_State(CState_Training02_Idle::Create(m_pFsmCom, this, IDLE, &Desc));
	m_pFsmCom->Add_State(CState_Training_Grogy::Create(m_pFsmCom, this, GROGY, &Desc));
	m_pFsmCom->Add_State(CState_Training_HitFatal::Create(m_pFsmCom, this, HITFATAL, &Desc));

	m_pFsmCom->Set_State(IDLE);

	return S_OK;



}

CMonster_Training02* CMonster_Training02::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_Training02* pInstance = new CMonster_Training02(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMonster_Training02"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CPawn* CMonster_Training02::Clone(void* pArg)
{
	CMonster_Training02* pInstance = new CMonster_Training02(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMonster_Training02"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_Training02::Free()
{
	__super::Free();

}