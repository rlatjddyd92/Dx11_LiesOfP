#include "stdafx.h"
#include "Weapon_FlameSword.h"

#include "Player.h"
#include "Monster.h"

#include "GameInstance.h"

// 24-12-06 김성용
// 내구도 조정 함수 연결을 위한 헤더 추가 
#include "GameInterface_Controller.h"

CWeapon_FlameSword::CWeapon_FlameSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_FlameSword::CWeapon_FlameSword(const CWeapon_FlameSword& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CWeapon_FlameSword::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_FlameSword::Initialize(void* pArg)
{
	PLAYER_WAPON_DESC* pDesc = static_cast<PLAYER_WAPON_DESC*>(pArg);
	m_pPlayer = pDesc->pPlayer;
	if (nullptr == m_pPlayer)
		return E_FAIL;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_strObjectTag = TEXT("PlayerWeapon");
	m_fDamageAmount = 10.f;

	m_isActive = false;
	m_pColliderCom->IsActive(false);

	return S_OK;
}

void CWeapon_FlameSword::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

}

void CWeapon_FlameSword::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	__super::Update(fTimeDelta);

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CWeapon_FlameSword::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);

#endif
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

	m_pGameInstance->Add_ColliderList(m_pColliderCom);
}

HRESULT CWeapon_FlameSword::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_FlameSword::Render_LightDepth()
{
	if (FAILED(__super::Render_LightDepth()))
		return E_FAIL;

	return S_OK;
}

void CWeapon_FlameSword::OnCollisionEnter(CGameObject* pOther)
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
			pOther->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio);

			// 24-12-06 김성용
				// 무기 사용 시, 내구도 감소 
			GET_GAMEINTERFACE->Add_Durable_Weapon(-1.f);

			Play_HitSound(pMonster->Get_HitType());
		}
	}
}

void CWeapon_FlameSword::OnCollisionStay(CGameObject* pOther)
{
}

void CWeapon_FlameSword::OnCollisionExit(CGameObject* pOther)
{
}

void CWeapon_FlameSword::Play_HitSound(HIT_TYPE eType)
{
	_wstring strSoundKey{};
	_wstring strWAV = TEXT(".wav");
	_tchar szBuffer[10];

	_int iRand = rand() % 3 + 1;
	
	if (ATK_STRONG == m_eAttackStrength)
	{
		_itow_s(iRand, szBuffer, 10);
		strSoundKey = TEXT("SE_PC_SK_Hit_Fire_Common_0");
		strSoundKey = strSoundKey + szBuffer + strWAV;
	}
	else if (ATK_NORMAL == m_eAttackStrength
		|| ATK_WEAK == m_eAttackStrength)
	{
		_itow_s(iRand, szBuffer, 10);
		strSoundKey = TEXT("SE_PC_SK_Hit_Fire_M_0");
		strSoundKey = strSoundKey + szBuffer + strWAV;
	}

	m_pSoundCom[WEP_SOUND_EFFECT2]->Play2D(strSoundKey.c_str(), &g_fEffectVolume);
}

HRESULT CWeapon_FlameSword::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FlameSword"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.15f, 0.15f, 0.75f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, -0.7f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);


	return S_OK;
}

CWeapon_FlameSword* CWeapon_FlameSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_FlameSword* pInstance = new CWeapon_FlameSword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Scissor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_FlameSword::Clone(void* pArg)
{
	CWeapon_FlameSword* pInstance = new CWeapon_FlameSword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Scissor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_FlameSword::Free()
{
	__super::Free();
}
