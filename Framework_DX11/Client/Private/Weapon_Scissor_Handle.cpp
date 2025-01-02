#include "stdafx.h"
#include "Weapon_Scissor_Handle.h"
#include "Player.h"
#include "Weapon_Scissor_Blade.h"
#include "Monster.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

#include "Effect_Manager.h"
#include "Effect_Container.h"

CWeapon_Scissor_Handle::CWeapon_Scissor_Handle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWeapon{ pDevice, pContext }
{
}

CWeapon_Scissor_Handle::CWeapon_Scissor_Handle(const CWeapon_Scissor_Handle& Prototype)
	: CWeapon{ Prototype }
{
}

HRESULT CWeapon_Scissor_Handle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Scissor_Handle::Initialize(void* pArg)
{
	SCISSOR_DESC* pDesc = static_cast<SCISSOR_DESC*>(pArg);

	m_pPlayer = pDesc->pPlayer;
	if (nullptr == m_pPlayer)
		return E_FAIL;

	m_eType = pDesc->eScissorType;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Blade()))
		return E_FAIL;

	if (FAILED(Ready_Effect()))
		return E_FAIL;

	m_strObjectTag = TEXT("PlayerWeapon");
	m_fDamageAmount = 4.f;

	m_isActive = false;
	m_pColliderCom->IsActive(false);

	m_pBladeMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Blade_B");

	return S_OK;
}

void CWeapon_Scissor_Handle::Priority_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	__super::Priority_Update(fTimeDelta);
}

void CWeapon_Scissor_Handle::Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;


	//Active_Effect(EFFECT_BUFF, true);
	__super::Update(fTimeDelta);

	m_pBlade->Update(fTimeDelta);

	m_pColliderCom->Update(&m_WorldMatrix);
}

void CWeapon_Scissor_Handle::Late_Update(_float fTimeDelta)
{
	if (!m_isActive)
		return;

	__super::Late_Update(fTimeDelta);

	m_pBlade->Late_Update(fTimeDelta);


	m_pGameInstance->Add_ColliderList(m_pColliderCom);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);

#endif
	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RG_SHADOWOBJ, this);

}

HRESULT CWeapon_Scissor_Handle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Scissor_Handle::Render_LightDepth()
{
	if (FAILED(__super::Render_LightDepth()))
		return E_FAIL;

	return S_OK;
}

void CWeapon_Scissor_Handle::Effect_Priority_Update(_float fTimeDelta)
{
	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Priority_Update(fTimeDelta);
	}
}

void CWeapon_Scissor_Handle::Effect_Update(_float fTimeDelta)
{
	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Update(fTimeDelta);
	}
}

void CWeapon_Scissor_Handle::Effect_Late_Update(_float fTimeDelta)
{

	for (auto& pEffect : m_Effects)
	{
		if (!pEffect->Get_Dead())
			pEffect->Late_Update(fTimeDelta);
	}
}

void CWeapon_Scissor_Handle::OnCollisionEnter(CGameObject* pOther)
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

			_Vec3 vHitPos = {};

			if (!m_pBladeMatrix)
				vHitPos = m_BladeWorldMatrix.Translation();
			else
				vHitPos = m_WorldMatrix.Translation();

			if (pMonster->Calc_DamageGain(m_fDamageAmount * m_fDamageRatio, vHitPos, HIT_METAL, m_iAtkStrength, this))
			{
				_Vec3 vPlayerLook = (_Vec3)m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_LOOK);
				vPlayerLook.Normalize();

				CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Slash_Normal"),
					(_Vec3)pMonster->Calc_CenterPos(), vPlayerLook);

				CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("Player_Attack_Blood_Scissor"),
					m_pParentMatrix, m_pSocketMatrix);

				// 24-12-06 김성용
				// 무기 사용 시, 내구도 감소 
				GET_GAMEINTERFACE->Add_Durable_Weapon(-5.f);

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

void CWeapon_Scissor_Handle::OnCollisionStay(CGameObject* pOther)
{
}

void CWeapon_Scissor_Handle::OnCollisionExit(CGameObject* pOther)
{
}

void CWeapon_Scissor_Handle::Play_HitSound(HIT_TYPE eType)
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

HRESULT CWeapon_Scissor_Handle::Ready_Components()
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (m_eType == SCISSOR_LEFT)
	{
		/* FOR.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Scissor_Left_Hnd"),
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
	}
	else if (m_eType == SCISSOR_RIGHT)
	{
		/* FOR.Com_Model */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Scissor_Right_Hnd"),
			TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
			return E_FAIL;

		/* FOR.Com_Collider */
		CBounding_OBB::BOUNDING_OBB_DESC			ColliderDesc{};
		ColliderDesc.vExtents = _float3(0.1f, 0.1f, 0.75f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, 0.6f);
		ColliderDesc.vAngles = _float3(0.f, 0.f, 0.f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
			return E_FAIL;
	}


	m_pColliderCom->Set_Owner(this);


	return S_OK;
}

HRESULT CWeapon_Scissor_Handle::Ready_Blade()
{
	CWeapon_Scissor_Handle::SCISSOR_DESC ScissorDesc;
	ScissorDesc.pParentWorldMatrix = &m_WorldMatrix;
	ScissorDesc.pSocketBoneMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr("BN_Blade");

	if (m_eType == SCISSOR_LEFT)
	{
		ScissorDesc.eScissorType = SCISSOR_LEFT;
		m_pBlade = dynamic_cast<CWeapon_Scissor_Blade*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Scissor_Blade"), &ScissorDesc));
	}
	else if (m_eType == SCISSOR_RIGHT)
	{
		ScissorDesc.eScissorType = SCISSOR_RIGHT;
		m_pBlade = dynamic_cast<CWeapon_Scissor_Blade*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon_Scissor_Blade"), &ScissorDesc));
		if (nullptr == m_pBlade)
			return E_FAIL;
	}

	if (nullptr == m_pBlade)
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon_Scissor_Handle::Ready_Effect()
{
	if (FAILED(__super::Ready_Effect()))
		return E_FAIL;

	m_Effects.resize(EFFECT_END);

	if (m_eType == SCISSOR_LEFT)
	{
		m_Effects[EFFECT_BASE] = m_pEffect_Manager->Clone_Effect(TEXT("Player_Attack_Scissor_Slash"), m_pParentMatrix,
			m_pSocketMatrix);

		m_Effects[EFFECT_LINKSLASH1] = m_pEffect_Manager->Clone_Effect(TEXT("Player_Attack_Scissor_Left_LinkSlash_First"), m_pParentMatrix,
			m_pSocketMatrix);

		m_Effects[EFFECT_BUFF] = m_pEffect_Manager->Clone_Effect(TEXT("Player_Grind_Scissor_Left"), m_pParentMatrix,
			m_pSocketMatrix);
	}
	else
	{
		m_Effects[EFFECT_BASE] = m_pEffect_Manager->Clone_Effect(TEXT("Player_Attack_Scissor_Right_Slash"), m_pParentMatrix,
			m_pSocketMatrix);

		m_Effects[EFFECT_LINKSLASH1] = m_pEffect_Manager->Clone_Effect(TEXT("Player_Attack_Scissor_Right_LinkSlash_First"), m_pParentMatrix,
			m_pSocketMatrix);

		m_Effects[EFFECT_BUFF] = m_pEffect_Manager->Clone_Effect(TEXT("Player_Grind_Scissor_Right"), m_pParentMatrix,
			m_pSocketMatrix);
	}

	return S_OK;
}

CWeapon_Scissor_Handle* CWeapon_Scissor_Handle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWeapon_Scissor_Handle* pInstance = new CWeapon_Scissor_Handle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWeapon_Scissor_Handle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CWeapon_Scissor_Handle::Clone(void* pArg)
{
	CWeapon_Scissor_Handle* pInstance = new CWeapon_Scissor_Handle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWeapon_Scissor_Handle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon_Scissor_Handle::Free()
{
	__super::Free();

	Safe_Release(m_pBlade);
}
