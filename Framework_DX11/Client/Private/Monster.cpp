#include "stdafx.h"
#include "..\Public\Monster.h"

#include "GameInstance.h"
#include "Player.h"
#include "Weapon.h"

#include "GameInterface_Controller.h"
#include "Fsm.h"

#include "Dissolve_Effect.h"


CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPawn{ pDevice, pContext }
{
}

CMonster::CMonster(const CMonster& Prototype)
	: CPawn{ Prototype }
{
}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	CGameObject::GAMEOBJECT_DESC		Desc{};
	if (pArg != nullptr)
	{
		CGameObject::GAMEOBJECT_DESC* pDesc = static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);
		memcpy(&Desc, pDesc, sizeof(CGameObject::GAMEOBJECT_DESC));
	}

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;
	
	m_strObjectTag = TEXT("Monster");

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{
	//플레이어 위치 업데이트
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Player(LEVEL_GAMEPLAY));
	if (m_pGameInstance->Find_Player(LEVEL_GAMEPLAY) != nullptr)
	{
		m_vPosTarget = pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	}
	//그로기 체크
	if (m_eStat.fGrogyPoint >= m_eStat.fMaxGrogyPoint)
	{
		m_eStat.bWeakness = true;
	}
	//플레이어 사망 체크
	if (pPlayer->Get_IsDieState())
	{
		m_bTargetDead = true;
	}
	else
	{
		m_bTargetDead = false;
	}
	//페이탈 연타 방지 체크
	if (!m_bBackAttackCtr)
	{
		m_fFatalTimeStack += fTimeDelta;
		if (m_fFatalTimeStack >= m_fFatalDelay)
		{
			m_bBackAttackCtr = true;
		}
	}


	if (!m_isBoss)
	{
		if (!m_bBlockDead)
		{
			if (!m_bDieState && m_eStat.fHp <= 0.f)
			{
				GET_GAMEINTERFACE->Set_OnOff_OrthoUI(false, this);
				m_bDieState = true;
				m_pFsmCom->Change_State(DIE);

				m_pRigidBodyCom->Set_Kinematic(false);
				Reset_Debuff();

				//에르고 추가
				pPlayer->Get_Player_Stat_Adjust()->iErgo += m_iErgoPoint;
			}
		}
	}
}

void CMonster::Update(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

#ifdef _DEBUG
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
#endif
}

void CMonster::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);

	/*CComponent*		pTargetCollider = m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"));
	if (nullptr == pTargetCollider)
		return;*/

	//CComponent* pTargetCollider = m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0, CPlayer::PART_WEAPON);
	//if (nullptr == pTargetCollider)
	//	return;
	//
	//
	//m_pColliderCom->Intersect(dynamic_cast<CCollider*>(pTargetCollider));
	//
	if (true == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

#ifdef _DEBUG
		//m_pGameInstance->Add_DebugObject(m_pColliderCom);
#endif
	}
}

HRESULT CMonster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;


	return S_OK;
}

HRESULT CMonster::Render_LightDepth()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeViewMatrix", m_pGameInstance->Get_CascadeViewMatirx(), 3)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeProjMatrix", m_pGameInstance->Get_CascadeProjMatirx(), 3)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
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

_Vec4 CMonster::Get_TargetDir()
{
	_Vec4 vDir = m_vPosTarget - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vDir.y = 0.f;

	if (vDir.Length() == 0)
	{
		return _Vec4{0, 0, -1, 1};
	}
	return vDir;
}

void CMonster::Look_Player()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	if (!(XMVectorGetX(vPos) == XMVectorGetX(m_vPosTarget))
		&& !(XMVectorGetZ(vPos) == XMVectorGetZ(m_vPosTarget)))
	{
		m_pTransformCom->LookAt(XMVectorSetY(m_vPosTarget, 0.f));
	}

}

_float CMonster::Calc_Distance_XZ()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDist = XMVectorGetX(XMVector3Length(XMVectorSetY(vPos, 0) - XMVectorSetY(m_vPosTarget, 0)));

	//X, Y 값 플레이어 비교로 위치구하기
	return fDist;
}

void CMonster::Active_Debuff(_int iIndex, _float fDebuffRatio)
{
}

void CMonster::DeActive_Debuff(_int iIndex)
{
}

void CMonster::OnCollisionEnter(CGameObject* pOther)
{
	//if (pOther->Get_Tag() == TEXT("PlayerWeapon"))
	//{
	//	CWeapon* pWeapon = dynamic_cast<CWeapon*>(pOther);
	//	
	//	m_fHp -= pWeapon->Get_AtkDmg();
	//}

}

void CMonster::OnCollisionStay(CGameObject* pOther)
{
	int a = 0;
}

void CMonster::OnCollisionExit(CGameObject* pOther)
{
	int a = 0;
}

const _Matrix* CMonster::Get_BoneCombinedMat(_uint iBoneIndex)
{
	return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(iBoneIndex);
}

_bool CMonster::Calc_DamageGain(_float fAtkDmg, _Vec3 vHitPos, _uint iHitType, _uint iAttackStrength, CGameObject* pAttacker)
{
	if (m_bDieState)
	{
		return false;
	}

	m_eStat.fHp -= fAtkDmg;
	m_eStat.fAtkDmg = fAtkDmg;

	if (!m_isBoss)
	{
		if (iAttackStrength >= ATTACK_STRENGTH::ATK_STRONG && m_pFsmCom->Get_CurrentState() != KNOCKBACK &&
			m_pFsmCom->Get_CurrentState() != HITFATAL && m_pFsmCom->Get_CurrentState() != GROGY)
		{
			++m_iKnockBackCount;
		}
	}

	if (m_eStat.bWeakness && m_pFsmCom->Get_CurrentState() != HITFATAL && m_pFsmCom->Get_CurrentState() != GROGY)
	{
		if (iAttackStrength >= ATTACK_STRENGTH::ATK_NORMAL)
		{
			m_eStat.bWeakness = false;
			m_eStat.bFatalAttack = true;

			if (m_bHaveGrogy)
			{
				m_pFsmCom->Change_State(GROGY);
			}
			else
			{
				m_pFsmCom->Change_State(KNOCKBACK);
			}
			m_bDiscover = true;
			return true;
		}
	}

	if (!m_bDiscover)
	{
		if (m_pFsmCom->Get_CurrentState() != HITFATAL)
		{
			m_bDiscover = true;
			m_pFsmCom->Change_State(KNOCKBACK);
			m_bDiscover = true;
			return true;
		}
	}

	if (iAttackStrength == ATTACK_STRENGTH::ATK_LAST)
	{
		m_bFatalAttacked = true;
		m_eStat.bFatalAttack = false;
		m_bDiscover = true;
	}

	if (!m_isBoss)
	{
		if (m_iKnockBackCount >= m_iKnockBackResist)
		{
			m_pFsmCom->Change_State(KNOCKBACK);
			m_iKnockBackCount = 0;
			return true;
		}
	}

	if (!m_bDiscover)
	{
		m_bDiscover = true;
	}

	return true;
}

void CMonster::Increase_GroggyPoint(_float fGroggy)
{
	if (m_pFsmCom->Get_CurrentState() == GROGY
		|| m_pFsmCom->Get_CurrentState() == HITFATAL)
		return;

		m_eStat.fGrogyPoint += fGroggy;
}

void CMonster::Reset_GroggyPoint()
{
	m_eStat.fGrogyPoint = 0;
	m_eStat.bWeakness = false;
}

void CMonster::Start_Fatal()
{
	m_pFsmCom->Change_State(HITFATAL);
}

void CMonster::SetUp_Act()
{
	GET_GAMEINTERFACE->Set_OnOff_OrthoUI(true, this);
	m_bFirstMeetCheck = true;
}

void CMonster::Calc_DebuffGain(_uint iDebuffType, _float fDebuffDuration)
{
	if (m_isBoss)
	{
		return;

	}

	if (m_bDebuffed[iDebuffType])
	{
		if (m_fDebuffDuration[iDebuffType] < fDebuffDuration)
		{
			m_fDebuffDuration[iDebuffType] = fDebuffDuration;
		}
	}
	else
	{
		m_bDebuffed[iDebuffType] = true;

		m_fDebuffDuration[iDebuffType] = fDebuffDuration;
		//이펙트를 활성화
		On_SurfaceEffect(iDebuffType, true);
	}
}

void CMonster::Update_Debuff(_float fTimeDelta)
{
	for (_uint i = 0; i < SURFACE_END; ++i)
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
				m_bDebuffed[i] = false;
				On_SurfaceEffect(i, false);
			}
			 
		}
	}
}

void CMonster::Reset_Debuff()
{
	for (_uint i = 0; i < SURFACE_END; ++i)
	{
		m_bDebuffed[i] = false;
		m_fDebuffDuration[i] = 0.f;
		On_SurfaceEffect(i, false);
	}
}

void CMonster::On_SurfaceEffect(_uint iIndex, _bool bOn)
{
	if (m_SurfaceEffect.size() <= iIndex || !m_SurfaceEffect[iIndex])
		return;

	if (bOn != m_SurfaceEffect[iIndex]->Get_On())
		m_SurfaceEffect[iIndex]->Set_On(bOn);
}


HRESULT CMonster::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Ready_FSM()
{
	/* FOR.Com_FSM */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		TEXT("Com_FSM"), reinterpret_cast<CComponent**>(&m_pFsmCom))))
		return E_FAIL;



	return S_OK;
}

void CMonster::Free()
{
	__super::Free();

	for (auto& Effect : m_SurfaceEffect)
		Safe_Release(Effect);
	m_SurfaceEffect.clear();

}