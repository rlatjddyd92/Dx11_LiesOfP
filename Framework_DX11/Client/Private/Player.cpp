#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "Camera.h"
#include "Weapon.h"

#include "Effect_Manager.h"
#include "Effect_Container.h"

#include "State_Player_Hit.h"
#include "State_Player_Parry.h"
#include "State_Player_Heal.h"
#include "State_Player_ChangeWeapon.h"

#include "State_Player_OH_Idle.h"
#include "State_Player_OH_Walk.h"
#include "State_Player_OH_Run.h"
#include "State_Player_OH_Sprint.h"
#include "State_Player_OH_Guard.h"
#include "State_Player_OH_Dash.h"

#include "State_Player_TH_Idle.h"
#include "State_Player_TH_Walk.h"
#include "State_Player_TH_Run.h"
#include "State_Player_TH_Sprint.h"
#include "State_Player_TH_Guard.h"
#include "State_Player_TH_Dash.h"

#include "State_Player_Rapier_LAttack00.h"
#include "State_Player_Rapier_LAttack01.h"
#include "State_Player_Rapier_RAttack00.h"
#include "State_Player_Rapier_Charge.h"
#include "State_Player_Rapier_Fatal.h"

#include "State_Player_Flame_LAttack00.h"
#include "State_Player_Flame_LAttack01.h"
#include "State_Player_Flame_RAttack00.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPawn{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer & Prototype)
	: CPawn{ Prototype }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECT_DESC		Desc{};

	Desc.fSpeedPerSec = 10.0f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Weapon()))
		return E_FAIL;

	if (FAILED(Ready_FSM()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(2.f, 0.f, 0.f, 1.f));

	m_pPlayerCamera = m_pGameInstance->Find_Camera(LEVEL_GAMEPLAY);

	// 임시 루트본 설정
	m_pModelCom->Set_UFBIndices(UFB_ROOT, 2);
	m_pModelCom->Set_UFBIndices(UFB_BOUNDARY_UPPER, 5);
	m_pModelCom->Update_Boundary();

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	if (m_isGuard)
	{
		m_fGuardTime = fTimeDelta;
	}

	for (auto& pEffect : m_EffectList)
	{
		pEffect->Priority_Update(fTimeDelta);
	}
	//업데이트에서 생성하니 업데이트 이전에 비우기
	for (auto& pEffect : m_EffectList)
	{
		pEffect->Late_Update(fTimeDelta);
	}

	m_EffectList.clear();
	m_EvKeyList.clear();

	m_pWeapon[m_eWeaponType]->Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
	m_pFsmCom->Update(fTimeDelta);

	m_vCurRootMove = m_pModelCom->Play_Animation(fTimeDelta, &m_bEndAnim, &m_EvKeyList);


	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (true)//조건을 애니메이션이 끝났을때 or 변경 되었을때로
	{
		m_vCurRootMove = XMVector3TransformNormal(m_vCurRootMove, m_pTransformCom->Get_WorldMatrix());

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + m_vCurRootMove);
	}

	for (auto& EvKey : m_EvKeyList)
	{

		//if (EvKey.eEvent_type == EVENT_KEYFRAME::ET_ONCE)
		//{
		//	auto Effect = m_Effects.find(EvKey.iEffectNum);
		//	if (Effect == m_Effects.end())
		//	{
		//		CEffect_Container::EFFECT_DESC EffectDesc = {};
		//		EffectDesc.fRotationPerSec = XMConvertToRadians(90.f);
		//		EffectDesc.fSpeedPerSec = 1.f;
		//		EffectDesc.iLevelIndex = LEVEL_GAMEPLAY;
		//		EffectDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		//		EffectDesc.pSocketMatrix = (_Matrix*)m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(EvKey.iBoneIndex);
		//		
		//		CEffect_Manager::Get_Instance()->Clone_Effect(CEffect_Manager::EFFECT_POWER_HIT, &EffectDesc);
		//	}
		//}
		//else if (EvKey.eEvent_type == EVENT_KEYFRAME::ET_REPET)
		//{
		//	CEffect_Container* pEffectCon;
		//	auto Effect = m_Effects.find(EvKey.iEffectNum);
		//	if (Effect == m_Effects.end())
		//	{
		//		CEffect_Container::EFFECT_DESC EffectDesc = {};
		//		EffectDesc.fRotationPerSec = XMConvertToRadians(90.f);
		//		EffectDesc.fSpeedPerSec = 1.f;
		//		EffectDesc.iLevelIndex = LEVEL_GAMEPLAY;
		//		EffectDesc.vScale = _Vec3{1, 1, 1};
		//		EffectDesc.vPos = _Vec3{ 0, 0, 0 };
		//		EffectDesc.vRotation = _Vec3{ 0, 0, 0 };
		//		EffectDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
		//		EffectDesc.pSocketMatrix = (_Matrix*)m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(EvKey.iBoneIndex);
		//		pEffectCon =	CEffect_Manager::Get_Instance()->Clone_Effect(CEffect_Manager::EFFECT_POWER_HIT, &EffectDesc);
		//		m_Effects.emplace(EvKey.iEffectNum, pEffectCon);
		//	}
		//	else
		//	{
		//		pEffectCon = Effect->second;
		//	}
		//	m_EffectList.push_back(pEffectCon);
		//}
	}

	for (auto& pEffect : m_EffectList)
	{
		pEffect->Update(fTimeDelta);
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

	m_pWeapon[m_eWeaponType]->Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	//if (GetKeyState(VK_LBUTTON) & 0x8000)
	//{
	//	_float3		vPickPos;
	//	if (true == m_pGameInstance->Picking(&vPickPos))
	//		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPickPos), 1.f));		
	//}


	m_pRigidBodyCom->Update(fTimeDelta);
	m_pNavigationCom->SetUp_OnCell(m_pTransformCom, 0.f, fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
	m_pGameInstance->Add_DebugObject(m_pNavigationCom);
#endif

	m_pWeapon[m_eWeaponType]->Late_Update(fTimeDelta);
}

HRESULT CPlayer::Render()
{
	if (FAILED(Bind_WorldViewProj()))
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

	return S_OK;

#ifdef _DEBUG
	m_pColliderCom->Render();
	m_pNavigationCom->Render();
#endif


	if (FAILED(m_pWeapon[m_eWeaponType]->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Render_LightDepth()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeViewMatrix", m_pGameInstance->Get_CascadeViewMatirx(), 3)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeProjMatrix", m_pGameInstance->Get_CascadeProjMatirx(), 3)))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", (_uint)i);


		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render((_uint)i)))
			return E_FAIL;
	}

	return S_OK;
}

void CPlayer::Move_Dir(_Vec4 vDir, _float fTimeDelta, _bool isTurn)
{
	if(isTurn)
		m_pTransformCom->LookAt_Lerp_NoHeight(vDir, 30.0f, fTimeDelta);
	m_pRigidBodyCom->Set_Velocity((_Vec3(vDir * m_fMoveSpeed)));
}

_Vec4 CPlayer::Calculate_Direction_Straight()
{
	_vector vCameraLook = m_pPlayerCamera->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK);
	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::STATE_UP));
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vCameraLook));

	_vector vDir = XMVector3Normalize(XMVector3Cross(vRight, vUp));

	return vDir;
}

_Vec4 CPlayer::Calculate_Direction_Right()
{
	_vector vCameraLook = XMVector3Normalize(m_pPlayerCamera->Get_Transform()->Get_State(CTransform::STATE::STATE_LOOK));
	_vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::STATE_UP));

	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vCameraLook));

	return vRight;
}

void CPlayer::Change_Weapon()
{
	m_pWeapon[m_eWeaponType]->Appear();
}

_uint CPlayer::Change_WeaponType()
{
	m_pWeapon[m_eWeaponType]->Disappear();
	m_eWeaponType = WEAPON_TYPE((m_eWeaponType + 1) % WEP_END);
	return m_eWeaponType;
}

HRESULT CPlayer::Ready_Weapon()
{
	CWeapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Weapon_R");

	m_pWeapon[WEP_RAPIER] = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Rapier"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return E_FAIL;

	m_pWeapon[WEP_FLAME] = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_FlameSword"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return E_FAIL;

	m_pWeapon[WEP_SCISSOR] = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Scissor"), &WeaponDesc));
	if (nullptr == m_pWeapon)
		return E_FAIL;

	Change_Weapon();

	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NaviDesc{};
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.5f, 0.8f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	/* FOR.Com_RigidBody */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom))))
		return E_FAIL;
	m_pRigidBodyCom->Set_Owner(this);
	m_pRigidBodyCom->Set_IsFriction(true);
	m_pRigidBodyCom->Set_Friction(_float3(10.f, 0.f, 10.f));
	m_pRigidBodyCom->Set_IsGravity(false);
	m_pRigidBodyCom->Set_GravityScale(15.f);
	m_pRigidBodyCom->Set_VelocityLimit(_float3(25.f, 30.f, 25.f));
	m_pRigidBodyCom->Set_Navigation(m_pNavigationCom);

	return S_OK;
}

HRESULT CPlayer::Ready_FSM()
{
	/* FOR.Com_FSM */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFsmCom))))
		return E_FAIL;

	FSM_INIT_DESC Desc{};

	Desc.pIsEndAnim = &m_bEndAnim;
	Desc.pIsResetRootMove = &m_bResetRootMove;
	Desc.pPrevTrackPos = &m_fPrevTrackPos;


	m_pFsmCom->Add_State(CState_Player_Hit::Create(m_pFsmCom, this, HIT, &Desc));
	m_pFsmCom->Add_State(CState_Player_Parry::Create(m_pFsmCom, this, PARRY, &Desc));
	m_pFsmCom->Add_State(CState_Player_Heal::Create(m_pFsmCom, this, HEAL, &Desc));
	m_pFsmCom->Add_State(CState_Player_ChangeWeapon::Create(m_pFsmCom, this, CHANGEWEP, &Desc));

	m_pFsmCom->Add_State(CState_Player_OH_Idle::Create(m_pFsmCom, this, OH_IDLE, &Desc));
	m_pFsmCom->Add_State(CState_Player_OH_Walk::Create(m_pFsmCom, this, OH_WALK, &Desc));
	m_pFsmCom->Add_State(CState_Player_OH_Run::Create(m_pFsmCom, this, OH_RUN, &Desc));
	m_pFsmCom->Add_State(CState_Player_OH_Sprint::Create(m_pFsmCom, this, OH_SPRINT, &Desc));
	m_pFsmCom->Add_State(CState_Player_OH_Guard::Create(m_pFsmCom, this, OH_GUARD, &Desc));
	m_pFsmCom->Add_State(CState_Player_OH_Dash::Create(m_pFsmCom, this, OH_DASH, &Desc));

	m_pFsmCom->Add_State(CState_Player_TH_Idle::Create(m_pFsmCom, this, TH_IDLE, &Desc));
	m_pFsmCom->Add_State(CState_Player_TH_Walk::Create(m_pFsmCom, this, TH_WALK, &Desc));
	m_pFsmCom->Add_State(CState_Player_TH_Run::Create(m_pFsmCom, this, TH_RUN, &Desc));
	m_pFsmCom->Add_State(CState_Player_TH_Sprint::Create(m_pFsmCom, this, TH_SPRINT, &Desc));
	m_pFsmCom->Add_State(CState_Player_TH_Guard::Create(m_pFsmCom, this, TH_GUARD, &Desc));
	m_pFsmCom->Add_State(CState_Player_TH_Dash::Create(m_pFsmCom, this, TH_DASH, &Desc));

	m_pFsmCom->Add_State(CState_Player_Rapier_LAttack00::Create(m_pFsmCom, this, RAPIER_LATTACK0, &Desc));	// 좌클릭 공격1
	m_pFsmCom->Add_State(CState_Player_Rapier_LAttack01::Create(m_pFsmCom, this, RAPIER_LATTACK1, &Desc));	// 좌클릭 공격2
	m_pFsmCom->Add_State(CState_Player_Rapier_RAttack00::Create(m_pFsmCom, this, RAPIER_RATTACK0, &Desc));	// 우클릭 공격
	m_pFsmCom->Add_State(CState_Player_Rapier_Charge::Create(m_pFsmCom, this, RAPIER_CHARGE, &Desc));	// 우클릭 차지공격
	m_pFsmCom->Add_State(CState_Player_Rapier_Fatal::Create(m_pFsmCom, this, RAPIER_FATAL, &Desc));	// F 페이탈아츠


	m_pFsmCom->Add_State(CState_Player_Flame_LAttack00::Create(m_pFsmCom, this, FLAME_LATTACK0, &Desc));	// 좌클릭 공격1
	m_pFsmCom->Add_State(CState_Player_Flame_LAttack01::Create(m_pFsmCom, this, FLAME_LATTACK1, &Desc));	// 좌클릭 공격1
	m_pFsmCom->Add_State(CState_Player_Flame_RAttack00::Create(m_pFsmCom, this, FLAME_RATTACK0, &Desc));	// 좌클릭 공격2

	m_pFsmCom->Set_State(OH_IDLE);

	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CPawn* CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto & Pair : m_Effects)
	{
		Safe_Release(Pair.second);
	}
	m_Effects.clear();

	for (_uint i = 0; i < WEP_END; ++i)
	{
		Safe_Release(m_pWeapon[i]);
	}

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
}
