#include "stdafx.h"
#include "Weapon_Scissor.h"
#include "Player.h"
#include "Monster.h"

#include "GameInstance.h"
#include "Effect_Manager.h"
#include "Effect_Container.h"

// 24-12-06 김성용
// 내구도 조정 함수 연결을 위한 헤더 추가 
#include "GameInterface_Controller.h"

CWeapon_Scissor::CWeapon_Scissor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_Scissor::CWeapon_Scissor(const CWeapon_Scissor& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CWeapon_Scissor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Scissor::Initialize(void* pArg)
{
	PLAYER_WAPON_DESC* pDesc = static_cast<PLAYER_WAPON_DESC*>(pArg);
	m_pPlayer = pDesc->pPlayer;
	if (nullptr == m_pPlayer)
		return E_FAIL;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Seperate()))
		return E_FAIL;

	if (FAILED(Ready_Effect()))
		return E_FAIL;

	m_strObjectTag = TEXT("PlayerWeapon");
	m_fDamageAmount = 10.f;

	m_isActive = false;
	m_isSeperate = false;
	m_pColliderCom->IsActive(false);

	m_pBladeMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Blade_B");

	return S_OK;
}

void CWeapon_Scissor::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	__super::Priority_Update(fTimeDelta);

	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Priority_Update(fTimeDelta);
	}

	m_pScissor_Sperate[0]->Effect_Priority_Update(fTimeDelta);
	m_pScissor_Sperate[1]->Effect_Priority_Update(fTimeDelta);
}

void CWeapon_Scissor::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (!m_isSeperate)
	{
		__super::Update(fTimeDelta);

		m_pColliderCom->Update(&m_WorldMatrix);
		m_pGameInstance->Add_ColliderList(m_pColliderCom);
	}
	else if (m_isSeperate)
	{
		m_pScissor_Sperate[0]->Update(fTimeDelta);
		m_pScissor_Sperate[1]->Update(fTimeDelta);
	}

	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Update(fTimeDelta);
	}

	m_pScissor_Sperate[0]->Effect_Update(fTimeDelta);
	m_pScissor_Sperate[1]->Effect_Update(fTimeDelta);
}

void CWeapon_Scissor::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	if (!m_isSeperate)
	{
		__super::Late_Update(fTimeDelta);

#ifdef _DEBUG
		m_pGameInstance->Add_DebugObject(m_pColliderCom);

#endif
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

	}
	else if (m_isSeperate)
	{
		m_pScissor_Sperate[0]->Late_Update(fTimeDelta);
		m_pScissor_Sperate[1]->Late_Update(fTimeDelta);
	}

	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Late_Update(fTimeDelta);
	}

	m_pScissor_Sperate[0]->Effect_Late_Update(fTimeDelta);
	m_pScissor_Sperate[1]->Effect_Late_Update(fTimeDelta);
}

HRESULT CWeapon_Scissor::Render()
{
	if (m_isSeperate)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Scissor::Render_LightDepth()
{
	if (FAILED(__super::Render_LightDepth()))
		return E_FAIL;

	return S_OK;
}

void CWeapon_Scissor::OnCollisionEnter(CGameObject* pOther)
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
			if (pMonster->Get_IsDieState())
				return;

			m_DamagedObjects.push_back(pOther);
			if (pMonster->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio))
			{
				_Vec3 vPlayerLook = (_Vec3)m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK);
				vPlayerLook.Normalize();

				if (m_eAttackStrength == ATK_STRONG)
				{
					m_pPlayer->Get_Camera()->Start_PosShake(0.45f, 0.2f);
					CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Slash_FatalAttak_1"),
						(_Vec3)pMonster->Calc_CenterPos(), m_vAttackDir);
				}
				else if (m_eAttackStrength == ATK_LAST)
				{
					m_pPlayer->Get_Camera()->Start_PosShake(0.6f, 0.25f);
					CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Slash_FatalAttak_2"),
						(_Vec3)pMonster->Calc_CenterPos(), m_vAttackDir);
				}
				else
				{
					CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Slash_Normal"),
						(_Vec3)pMonster->Calc_CenterPos(), m_vAttackDir);
				}

				CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Blood_Scissor"),
					m_pParentMatrix, m_pSocketMatrix);

				// 24-12-06 김성용
				// 무기 사용 시, 내구도 감소 
				GET_GAMEINTERFACE->Add_Durable_Weapon(-5.f);

				if (pMonster->Get_Status()->fHp > 0.f)
				{
					CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Monster_Impact"),
						(_Vec3)pMonster->Calc_CenterPos(), m_vAttackDir);
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

void CWeapon_Scissor::OnCollisionStay(CGameObject* pOther)
{
}

void CWeapon_Scissor::OnCollisionExit(CGameObject* pOther)
{
}

_bool CWeapon_Scissor::Active_Collider(_float fDamageRatio, _uint iHandIndex, _uint iHitType, _uint iAtkStrength)
{
	if (!m_isSeperate)
	{
		if (m_pColliderCom->IsActive())
			return false;

		m_fDamageRatio = fDamageRatio;
		m_pColliderCom->IsActive(true);
		m_iHitType = iHitType;
		m_iAtkStrength = iAtkStrength;
		m_DamagedObjects.clear();

		return true;
	}
	else
	{
		return m_pScissor_Sperate[iHandIndex]->Active_Collider(fDamageRatio);
	}

	return true;
}

void CWeapon_Scissor::DeActive_Collider(_uint iHandIndex)
{
	if (!m_isSeperate)
	{
		m_pColliderCom->IsActive(false);
	}
	else
	{
		m_pScissor_Sperate[iHandIndex]->DeActive_Collider();
	}
}

void CWeapon_Scissor::Play_Sound(WEP_SOUND_TYPE eType, const TCHAR* pSoundKey, _uint iHandIndex)
{
	if (!m_isSeperate)
	{
		m_pSoundCom[eType]->Play2D(pSoundKey, &g_fEffectVolume);
	}
	else
	{
		m_pScissor_Sperate[iHandIndex]->Play_Sound(eType, pSoundKey);
	}
}

void CWeapon_Scissor::Play_HitSound(HIT_TYPE eType)
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
			strSoundKey = TEXT("SE_PC_SK_Hit_Skin_Slice_L_0");
			strSoundKey = strSoundKey + szBuffer + strWAV;
			break;

		case HIT_METAL:
			iRand = rand() % 4 + 16;
			_itow_s(iRand, szBuffer, 10);
			strSoundKey = TEXT("SE_PC_SK_Hit_L_Metal_Slice_");
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
			strSoundKey = TEXT("SE_PC_SK_Hit_Skin_Slice_M_0");
			strSoundKey = strSoundKey + szBuffer + strWAV;
			break;

		case HIT_METAL:
			iRand = rand() % 4 + 1;
			_itow_s(iRand, szBuffer, 10);
			strSoundKey = TEXT("SE_PC_SK_Hit_Metal_Blood_Slice_0");
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
			strSoundKey = TEXT("SE_PC_SK_Hit_Skin_Slice_S_0");
			strSoundKey = strSoundKey + szBuffer + strWAV;
			break;

		case HIT_METAL:
			iRand = rand() % 4 + 1;
			_itow_s(iRand, szBuffer, 10);
			strSoundKey = TEXT("SE_PC_SK_Hit_Metal_Blood_Slice_0");
			strSoundKey = strSoundKey + szBuffer + strWAV;
			break;

		default:
			break;
		}
	}

	m_pSoundCom[WEP_SOUND_EFFECT2]->Play2D(strSoundKey.c_str(), &g_fEffectVolume);
}

void CWeapon_Scissor::Set_AttackStrength(ATTACK_STRENGTH eStrength)
{
	m_eAttackStrength = eStrength;

	m_pScissor_Sperate[0]->Set_AttackStrength(eStrength);
	m_pScissor_Sperate[1]->Set_AttackStrength(eStrength);
}

void CWeapon_Scissor::Active_Effect(const _uint& iType, _bool isLoop, _uint iHandIndex)
{
	if (!m_isSeperate)
	{
		__super::Active_Effect(iType, isLoop);
	}
	else
	{
		m_pScissor_Sperate[iHandIndex]->Active_Effect(iType, isLoop);
	}
}

void CWeapon_Scissor::DeActive_Effect(_uint iType, _uint iHandIndex)
{
	if (!m_isSeperate)
	{
		__super::DeActive_Effect(iType);
	}
	else
	{
		m_pScissor_Sperate[iHandIndex]->DeActive_Effect (iType);
	}
}

void CWeapon_Scissor::DeActive_AllEffect(_uint iHandIndex)
{
	__super::DeActive_AllEffect();
	m_pScissor_Sperate[0]->DeActive_AllEffect();
	m_pScissor_Sperate[0]->DeActive_AllEffect();
}

void CWeapon_Scissor::Change_SeperateMode()
{
	if (m_isSeperate)
		return;

	m_pColliderCom->IsActive(false);

	m_pScissor_Sperate[0]->IsActive(true);
	m_pScissor_Sperate[1]->IsActive(true);

	m_isSeperate = true;
}

void CWeapon_Scissor::Change_CombineMode(_bool isForce)
{
	if (!m_isSeperate)
		return;

	m_pScissor_Sperate[0]->IsActive(false);
	m_pScissor_Sperate[1]->IsActive(false);

	m_isSeperate = false;

	if (!isForce)
	{
		Active_Effect(EFFECT_COMBINE, false);
		m_pSoundCom[WEP_SOUND_WEAPON]->Play2D(TEXT("SE_PC_SK_Spark_Metal_07.wav"), &g_fEffectVolume);
	}

	m_pScissor_Sperate[0]->DeActive_AllEffect();
	m_pScissor_Sperate[1]->DeActive_AllEffect();
}

HRESULT CWeapon_Scissor::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	/* FOR.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Scissor_Combine"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* FOR.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.1f, 0.1f, 0.75f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, -0.6f);
	ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

HRESULT CWeapon_Scissor::Ready_Seperate()
{
	CWeapon_Scissor_Handle::SCISSOR_DESC ScissorDesc;
	ZeroMemory(&ScissorDesc, sizeof(CWeapon_Scissor_Handle::SCISSOR_DESC));
	ScissorDesc.pPlayer = m_pPlayer;

	ScissorDesc.eScissorType = CWeapon_Scissor_Handle::SCISSOR_LEFT; 
	ScissorDesc.pParentWorldMatrix = m_pParentMatrix;
	ScissorDesc.pSocketBoneMatrix = m_pSocketMatrix; //m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Blade");BN_Weapon_R

	m_pScissor_Sperate[CWeapon_Scissor_Handle::SCISSOR_LEFT] = dynamic_cast<CWeapon_Scissor_Handle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Scissor_Handle"), &ScissorDesc));
	if (nullptr == m_pScissor_Sperate[CWeapon_Scissor_Handle::SCISSOR_LEFT])
		return E_FAIL;

	ScissorDesc.eScissorType = CWeapon_Scissor_Handle::SCISSOR_RIGHT;
	ScissorDesc.pSocketBoneMatrix = m_pSocketMatrix2;
	m_pScissor_Sperate[CWeapon_Scissor_Handle::SCISSOR_RIGHT] = dynamic_cast<CWeapon_Scissor_Handle*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Scissor_Handle"), &ScissorDesc));
	if (nullptr == m_pScissor_Sperate[CWeapon_Scissor_Handle::SCISSOR_RIGHT])
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Scissor::Ready_Effect()
{
	if (FAILED(__super::Ready_Effect()))
		return E_FAIL;

	m_Effects.resize(EFFECT_END);

	m_Effects[EFFECT_BASE] = m_pEffect_Manager->Clone_Effect(TEXT("Player_Attack_Scissor_Slash"), m_pParentMatrix,
		m_pSocketMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_COMBINE] = m_pEffect_Manager->Clone_Effect(TEXT("Player_Attack_Scissor_Combine"), m_pParentMatrix,
		m_pSocketMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	m_Effects[EFFECT_LINKSLASH] = m_pEffect_Manager->Clone_Effect(TEXT("Player_Attack_Scissor_Left_LinkSlash_Second"), m_pParentMatrix,
		m_pSocketMatrix, _Vec3(0.f, 0.f, 0.f), _Vec3(0.f, 0.f, 0.f), _Vec3(1.f, 1.f, 1.f));

	return S_OK;
}

CWeapon_Scissor* CWeapon_Scissor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Scissor* pInstance = new CWeapon_Scissor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Scissor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_Scissor::Clone(void* pArg)
{
	CWeapon_Scissor* pInstance = new CWeapon_Scissor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Scissor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Scissor::Free()
{
	__super::Free();

	for (_uint i = 0; i < 2; ++i)
	{
		Safe_Release(m_pScissor_Sperate[i]);
	}
}
