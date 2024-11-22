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

	return S_OK;
}

void CSimonManus::Priority_Update(_float fTimeDelta)
{

	__super::Set_UpTargetPos();
	m_pWeapon->Priority_Update(fTimeDelta);
}

void CSimonManus::Update(_float fTimeDelta)
{

	m_pFsmCom->Update(fTimeDelta);
	 
	m_vCurRootMove = m_pModelCom->Play_Animation(fTimeDelta, nullptr);


	_Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_vCurRootMove = XMVector3TransformNormal(m_vCurRootMove, m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + m_vCurRootMove);

	if (KEY_TAP(KEY::B))
	{
		ChangePhase();
	}
	//for (auto& pColliderObj : m_pColliderObject)
	//	pColliderObj->Update(fTimeDelta);

	//for (auto& pCollider : m_pColliderCom)
	//	pCollider->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

	m_pWeapon->Update(fTimeDelta);
}

void CSimonManus::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

	for (_uint i = 0; i < TYPE_END; ++i)
	{
		//if (m_bColliderCtrs[i] != true)
		//{
		//	m_pColliderObject[i]->Late_Update(fTimeDelta);
		//}
	}
	m_pWeapon->Late_Update(fTimeDelta);

}

HRESULT CSimonManus::Render()
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


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}


#ifdef _DEBUG
	//m_pColliderCom[1]->Render();
	//for (auto& pColliderObj : m_pColliderObject)
	//	pColliderObj->Render();
#endif

	if (FAILED(m_pWeapon->Render()))
		return E_FAIL;

	return S_OK;
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

	return S_OK;
}

HRESULT CSimonManus::Ready_FSM()
{
	if (FAILED(__super::Ready_FSM()))
		return E_FAIL;

	FSMSTATE_DESC Desc{};
	
	Desc.pIsEndAnim = &m_bEndAnim;
	Desc.pIsResetRootMove =&m_bResetRootMove;
	Desc.pColliderCtrs = m_bColliderCtrs;
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

	m_pExtraFsmCom->Set_State(IDLE);
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
	//for (_uint i = 0; i < TYPE_END; ++i)
	//{
	//	Safe_Release(m_pColliderObject[i]);
	//}
	Safe_Release(m_pWeapon);
	Safe_Release(m_pExtraModelCom);

	if (m_pExtraFsmCom != nullptr)
	{
		m_pExtraFsmCom->Release_States();
	}
	Safe_Release(m_pExtraFsmCom);
	__super::Free();

}