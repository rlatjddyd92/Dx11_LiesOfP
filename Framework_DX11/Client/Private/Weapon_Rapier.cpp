#include "stdafx.h"
#include "Weapon_Rapier.h"

#include "Player.h"
#include "Monster.h"

#include "GameInstance.h"
#include "Effect_Manager.h"

// 24-12-06 김성용
// 내구도 조정 함수 연결을 위한 헤더 추가 
#include "GameInterface_Controller.h"

CWeapon_Rapier::CWeapon_Rapier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_Rapier::CWeapon_Rapier(const CWeapon_Rapier& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CWeapon_Rapier::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Rapier::Initialize(void* pArg)
{
	PLAYER_WAPON_DESC* pDesc = static_cast<PLAYER_WAPON_DESC*>(pArg);
	m_pPlayer = pDesc->pPlayer;
	if (nullptr == m_pPlayer)
		return E_FAIL;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Effect()))
		return E_FAIL;

	m_strObjectTag = TEXT("PlayerWeapon");
	m_pColliderCom->Set_Owner(this);
	m_fDamageAmount = 5.f;

	m_pColliderCom->IsActive(false);

	m_pBladeMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Blade_B");

	return S_OK;
}

void CWeapon_Rapier::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;


	__super::Priority_Update(fTimeDelta);
}

void CWeapon_Rapier::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	__super::Update(fTimeDelta);

	if (m_iAttackType != ATK_EFFECT_NOTHING)
	{
		if (m_vVelocity.Length() > 0.001f)
		{
			if (m_iAttackType == ATK_EFFECT_SPECIAL1)
			{
				Active_Effect(EFFECT_STORMSTAB1, true);
			}
			else if (m_iAttackType == ATK_EFFECT_SPECIAL2)
			{
				DeActive_Effect(EFFECT_STORMSTAB1);
				Active_Effect(EFFECT_STORMSTAB2, true);
			}
		}
		else
		{
			DeActive_Effect(EFFECT_STORMSTAB1);
			DeActive_Effect(EFFECT_STORMSTAB2);
		}
	}

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CWeapon_Rapier::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);


	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
}

HRESULT CWeapon_Rapier::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Rapier::Render_LightDepth()
{
	if (FAILED(__super::Render_LightDepth()))
		return E_FAIL;

	return S_OK;
}

void CWeapon_Rapier::OnCollisionEnter(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Monster"))
	{
		_bool bOverlapCheck = false;
		for (auto& pObj : m_DamagedObjects)
		{
			if (pObj == pOther)
			{
				bOverlapCheck = true;
				break;
			}
		}

		if (!bOverlapCheck)
		{
			CMonster* pMonster = dynamic_cast<CMonster*>(pOther);

			m_DamagedObjects.push_back(pOther);
			if (pMonster->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio))
			{
				_Vec3 vPlayerLook = (_Vec3)m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK);
				vPlayerLook.Normalize();

				CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Step_Normal"),
					(_Vec3)pMonster->Calc_CenterPos(), m_vAttackDir);

				CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Blood_Rapier"),
					m_pParentMatrix, m_pSocketMatrix);

				// 24-12-06 김성용
				// 무기 사용 시, 내구도 감소 
				GET_GAMEINTERFACE->Add_Durable_Weapon(-1.f);

				if (pMonster->Get_Status()->fHp > 0.f)
				{
					CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Impact"),
						(_Vec3)pMonster->Calc_CenterPos(), vPlayerLook);
				}
				else if (pMonster->Get_Status()->fHp <= 0.f)
				{
					CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Impact_Death"),
						(_Vec3)pMonster->Calc_CenterPos(), vPlayerLook);
				}
			}

			Play_HitSound(pMonster->Get_HitType());
		}
	}
}

void CWeapon_Rapier::OnCollisionStay(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Monster"))
	{

	}
}

void CWeapon_Rapier::OnCollisionExit(CGameObject* pOther)
{
	if (pOther->Get_Tag() == TEXT("Monster"))
	{

	}
}

void CWeapon_Rapier::Play_HitSound(HIT_TYPE eType)
{
	_wstring strSoundKey{};
	_wstring strWAV = TEXT(".wav");
	_tchar szBuffer[10];

	_int iRand = rand() % 3 + 1;

	if (ATK_STRONG == m_eAttackStrength)
	{
		switch (eType)
		{
		case HIT_CARCASS:
			_itow_s(iRand, szBuffer, 10);
			strSoundKey = TEXT("SE_PC_SK_Hit_Skin_Stab_L_0");
			strSoundKey = strSoundKey + szBuffer + strWAV;
			break;

		case HIT_METAL:
			_itow_s(iRand, szBuffer, 10);
			strSoundKey = TEXT("SE_PC_SK_Hit_L_Metal_Stab_0");
			strSoundKey = strSoundKey + szBuffer + strWAV;
			break;

		default:
			break;
		}
	}
	else if (ATK_NORMAL == m_eAttackStrength)
	{
		switch (eType)
		{
		case HIT_CARCASS:
			_itow_s(iRand, szBuffer, 10);
			strSoundKey = TEXT("SE_PC_SK_Hit_Skin_Stab_M_0");
			strSoundKey = strSoundKey + szBuffer + strWAV;
			break;

		case HIT_METAL:
			iRand = rand() % 4 + 1;
			_itow_s(iRand, szBuffer, 10);
			strSoundKey = TEXT("SE_PC_SK_Hit_Metal_Blood_Stab_0");
			strSoundKey = strSoundKey + szBuffer + strWAV;
			break;

		default:
			break;
		}
	}
	else if (ATK_WEAK == m_eAttackStrength)
	{
		switch (eType)
		{
		case HIT_CARCASS:
			_itow_s(iRand, szBuffer, 10);
			strSoundKey = TEXT("SE_PC_SK_Hit_Skin_Stab_S_0");
			strSoundKey = strSoundKey + szBuffer + strWAV;
			break;

		case HIT_METAL:
			_itow_s(iRand, szBuffer, 10);
			strSoundKey = TEXT("SE_PC_SK_Hit_M_Metal_Stab_0");
			strSoundKey = strSoundKey + szBuffer + strWAV;
			break;

		default:
			break;
		}
	}

	m_pSoundCom[WEP_SOUND_EFFECT2]->Play2D(strSoundKey.c_str(), &g_fEffectVolume);
}

HRESULT CWeapon_Rapier::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rapier"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.1f, 0.1f, 0.53f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, -0.5f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

HRESULT CWeapon_Rapier::Ready_Effect()
{
	if(FAILED(__super::Ready_Effect()))
		return E_FAIL;

	m_Effects.resize(EFFECT_END);

	m_Effects[EFFECT_STORMSTAB1] = m_pEffect_Manager->Clone_Effect(TEXT("Player_Attack_Rapier_StormStab_First"), m_pParentMatrix,
		m_pSocketMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_STORMSTAB2] = m_pEffect_Manager->Clone_Effect(TEXT("Player_Attack_Rapier_StormStab_Second"), m_pParentMatrix,
		m_pSocketMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	return S_OK;
}

CWeapon_Rapier* CWeapon_Rapier::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Rapier* pInstance = new CWeapon_Rapier(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Rapier"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_Rapier::Clone(void* pArg)
{
	CWeapon_Rapier* pInstance = new CWeapon_Rapier(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Rapier"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Rapier::Free()
{
	__super::Free();
}
