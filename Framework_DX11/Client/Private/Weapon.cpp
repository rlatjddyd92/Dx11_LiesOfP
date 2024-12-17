#include "stdafx.h"
#include "..\Public\Weapon.h"
#include "GameInstance.h"

#include "Player.h"

#include "Effect_Container.h"
#include "Effect_Manager.h"

CWeapon::CWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CWeapon::CWeapon(const CWeapon & Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon::Initialize(void * pArg)
{
	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_pSocketMatrix = pDesc->pSocketBoneMatrix;
	m_pSocketMatrix2 = pDesc->pSocketBoneMatrix2;
	m_pParentMatrix = pDesc->pParentWorldMatrix;

	m_pParentAtk = pDesc->pParentAtk;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	m_isActive = false;

	return S_OK;
}

void CWeapon::Priority_Update(_float fTimeDelta)
{
}

void CWeapon::Update(_float fTimeDelta)
{
	if (nullptr != m_pModelCom)
		m_pModelCom->Update_Bone();	

	_matrix		SocketMatrix = *m_pSocketMatrix;

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}
	m_OldWroldMatrix = m_WorldMatrix;
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));


	if (nullptr != m_pBladeMatrix)
	{
		SocketMatrix = *m_pBladeMatrix;
		for (size_t i = 0; i < 3; i++)
		{
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
		}

		m_BladeOldWroldMatrix = m_BladeWorldMatrix;
		XMStoreFloat4x4(&m_BladeWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(&m_WorldMatrix));

		_Vec3 vVelocity = m_BladeWorldMatrix.Translation() - m_BladeOldWroldMatrix.Translation();
		_float fVelocity = vVelocity.Length();
		if (fVelocity > 0.f)
		{
			m_vVelocity = m_BladeWorldMatrix.Translation() - m_BladeOldWroldMatrix.Translation();
			m_vAttackDir = m_vVelocity;
			m_vAttackDir.Normalize();
		}
	}
	else
	{
		m_vVelocity = m_WorldMatrix.Translation() - m_OldWroldMatrix.Translation();
		m_vAttackDir = m_vVelocity;
		m_vAttackDir.Normalize();
	}

	for (_uint i = 0; i < WEP_SOUND_END; ++i)
	{
		m_pSoundCom[i]->Update(fTimeDelta);
	}
}

void CWeapon::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	/*if (nullptr != m_pColliderCom)
		m_pGameInstance->Add_ColliderList(m_pColliderCom);*/
}

HRESULT CWeapon::Render()
{
	if (!m_isActive)
		return S_OK;

	if (FAILED(Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	//if (FAILED(Bind_OldWorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", &m_pGameInstance->Get_OldTransform(CPipeLine::D3DTS_VIEW))))
	//	return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &m_pGameInstance->Get_Far(), sizeof(_float))))
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

		// EMISSIVE
		if (nullptr != m_pModelCom->Find_Texture((_uint)i, TEXTURE_TYPE::EMISSIVE))
		{
			m_fEmissiveMask = 1.f;
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_EmessiveTexture", EMISSIVE, (_uint)i)))
				return E_FAIL;
		}
		else
		{
			m_fEmissiveMask = 0.f;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmessiveMask", &m_fEmissiveMask, sizeof(_float))))
			return E_FAIL;

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

	m_fEmissiveMask = 0.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fEmessiveMask", &m_fEmissiveMask, sizeof(_float))))
		return E_FAIL;
#ifdef _DEBUG
	if(nullptr != m_pColliderCom)
		m_pColliderCom->Render();
#endif

	return S_OK;
}

HRESULT CWeapon::Render_LightDepth()
{
	if (FAILED(Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
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

_bool CWeapon::Active_Collider(_float fDamageRatio, _uint iHandIndex, _uint iHitType, _uint iAtkStrength)
{
	if (m_pColliderCom->IsActive())
		return false ;

	m_fDamageRatio = fDamageRatio;
	m_pColliderCom->IsActive(true);
	m_iHitType = iHitType;
	m_iAtkStrength = iAtkStrength;
	m_DamagedObjects.clear();

	return true;
}

void CWeapon::DeActive_Collider(_uint iHandIndex)
{
	if (!m_pColliderCom->IsActive())
		return;

	m_pColliderCom->IsActive(false);
}

void CWeapon::Play_Sound(WEP_SOUND_TYPE eType, const TCHAR* pSoundKey, _uint iHandIndex)
{
	//_float fVolume = {};
	//switch (eType)
	//{
	//case SOUND_WEAPON:
	//	fVolume = g_fVoiceVolume;
	//	break;
	//case SOUND_EFFECT1:
	//case SOUND_EFFECT2:
	//	fVolume = ;
	//	break;
	//default:
	//	return;
	//}
	m_pSoundCom[eType]->Play2D(pSoundKey, &g_fEffectVolume);
}

void CWeapon::Active_Effect(const _uint& iType, _bool isLoop, _uint iHandIndex)
{
	if (isLoop)
	{
		if (!m_Effects[iType]->Get_Loop())
			m_Effects[iType]->Set_Loop(true);
	}
	else
	{
		m_Effects[iType]->Reset_Effects();
	}
}

void CWeapon::DeActive_Effect(_uint iType, _uint iHandIndex)
{
	m_Effects[iType]->Set_Loop(false);
}

void CWeapon::DeActive_AllEffect(_uint iHandIndex)
{
	for (auto& Effect : m_Effects)
	{
		Effect->Set_Loop(false);
	}
}

void CWeapon::Appear()
{
	m_pModelCom->Update_Bone();

	_matrix		SocketMatrix = *m_pSocketMatrix;

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}
	XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	m_isActive = true;
}

// 무조건 LateUpdate 전에 호출하기
void CWeapon::Disappear()
{
	m_pGameInstance->Add_ColliderList(m_pColliderCom);
	m_isActive = false;
}

const _Matrix* CWeapon::Get_BoneCombinedMatrix(_uint iBoneIndex)
{
	return m_pModelCom->Get_BoneCombindTransformationMatrix_Ptr(iBoneIndex);
}

HRESULT CWeapon::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_WeaponSound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_WeaponSound"), reinterpret_cast<CComponent**>(&m_pSoundCom[WEP_SOUND_WEAPON]))))
		return E_FAIL;
	m_pSoundCom[WEP_SOUND_WEAPON]->Set_Owner(this);

	/* FOR.Com_EffectSound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_EffectSound"), reinterpret_cast<CComponent**>(&m_pSoundCom[WEP_SOUND_EFFECT1]))))
		return E_FAIL;
	m_pSoundCom[WEP_SOUND_EFFECT1]->Set_Owner(this);

	/* FOR.Com_EffectSound2 */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_EffectSound2"), reinterpret_cast<CComponent**>(&m_pSoundCom[WEP_SOUND_EFFECT2]))))
		return E_FAIL;
	m_pSoundCom[WEP_SOUND_EFFECT2]->Set_Owner(this);

	return S_OK;
}

HRESULT CWeapon::Ready_Effect()
{
	m_pEffect_Manager = CEffect_Manager::Get_Instance();
	if (nullptr == m_pEffect_Manager)
		return E_FAIL;
	Safe_AddRef(m_pEffect_Manager);

	return S_OK;
}

HRESULT CWeapon::Bind_WorldMatrix(CShader* pShader, const _char* pContantName)
{
	return pShader->Bind_Matrix(pContantName, &m_WorldMatrix);
}

HRESULT CWeapon::Bind_OldWorldMatrix(CShader* pShader, const _char* pContantName)
{
	return pShader->Bind_Matrix(pContantName, &m_OldWroldMatrix);
}

void CWeapon::Free()
{
	__super::Free();

	if (true == m_isCloned)
	{
		for (auto& pEffect : m_Effects)
		{
			pEffect->Set_Cloned(false);
			Safe_Release(pEffect);
		}
		m_Effects.clear();
		Safe_Release(m_pEffect_Manager);
	}

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);	
	Safe_Release(m_pModelCom);

	for (_uint i = 0; i < WEP_SOUND_END; ++i)
	{
		Safe_Release(m_pSoundCom[i]);
	}
}
