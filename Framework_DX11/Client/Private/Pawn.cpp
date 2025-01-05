#include "Pawn.h"
#include "GameInstance.h"
#include "Effect_Manager.h"

CPawn::CPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CPawn::CPawn(const CPawn& Prototype)
	: CGameObject{ Prototype }
{

}

HRESULT CPawn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPawn::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_vRimLightColor = { 0.f,0.f,0.f, 0.f };
	return S_OK;
}

void CPawn::Priority_Update(_float fTimeDelta)
{
}

void CPawn::Update(_float fTimeDelta)
{
}

void CPawn::Late_Update(_float fTimeDelta)
{
}

HRESULT CPawn::Render()
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

		//RimLight
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimLight", &m_vRimLightColor, sizeof(_float4))))
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

	//RimLight 초기화
	_Vec4 vInitRimLight = _Vec4(0.f, 0.f, 0.f, 0.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimLight", &vInitRimLight, sizeof(_float4))))
		return E_FAIL;

#ifdef _DEBUG
	m_pColliderCom->Render();
	m_pNavigationCom->Render();
#endif

	return S_OK;
}

_bool CPawn::Calc_DamageGain(_float fAtkDmg, _Vec3 vHitPos, _uint iHitType, _uint iAttackStrength, CGameObject* pAttacker)
{

	//if () 피해를 입는지에관한 판단, 무적인지등
	//{
	//
	//}		//피해를 입는다면 true 반환, 

	m_eStat.fHp -= fAtkDmg;
	m_eStat.fAtkDmg = fAtkDmg;

	return true;
}

void CPawn::OnCollisionEnter(CGameObject* pOther)
{
}

void CPawn::OnCollisionStay(CGameObject* pOther)
{
}

void CPawn::OnCollisionExit(CGameObject* pOther)
{
}


void CPawn::Change_State(const _uint iState, void* pArg)
{
	m_pFsmCom->Change_State(iState, pArg);
}

void CPawn::Set_Animation(_uint iAnimIndex, _bool IsLoop)
{
	m_pModelCom->SetUp_Animation(iAnimIndex, IsLoop);
}

void CPawn::SetUp_Animation(_uint iAnimIndex, _bool IsLoop, _uint iStartFrame, _bool bEitherBoundary)
{
	m_pModelCom->SetUp_Animation(iAnimIndex, IsLoop, iStartFrame, bEitherBoundary);
}

void CPawn::SetUp_Animation_Boundry(_uint iAnimIndex, _bool IsLoop, _uint iStartFrame)
{
	m_pModelCom->SetUp_Animation_Boundary(iAnimIndex, IsLoop, iStartFrame);
}

void CPawn::Change_Animation(_uint iAnimIndex, _bool IsLoop, _float fDuration, _uint iStartFrame, _bool bEitherBoundary, _bool bSameChange)
{
	m_pModelCom->SetUp_NextAnimation(iAnimIndex, IsLoop, fDuration, iStartFrame, bEitherBoundary, bSameChange);
}

void CPawn::Change_Animation_Boundry(_uint iAnimIndex, _bool IsLoop, _float fDuration, _uint iStartFrame)
{
	// 바꿀 상체 애니메이션(하체가 주 애니메이션)
	m_pModelCom->SetUp_NextAnimation_Boundary(iAnimIndex, IsLoop, fDuration, iStartFrame);
}

_uint CPawn::Get_CurrentAnimIndex()
{
	return m_pModelCom->Get_CurrentAnimationIndex();
}

_uint CPawn::Get_CurrentAnimIndex_Boundry()
{
	return m_pModelCom->Get_CurrentAnimationIndex_Boundary();
}

_int CPawn::Get_Frame(_bool isBoundry)
{
	return m_pModelCom->Get_CurrentFrame(isBoundry);
}

_double CPawn::Get_CurrentTrackPos()
{
	return m_pModelCom->Get_CurrentTrackPosition();
}

_float CPawn::Get_CurrentTrackEnd()
{
	return m_pModelCom->Get_CurrentDuration();
}

_bool CPawn::Get_EndAnim(_int iAnimIndex, _bool bIsBoundary)
{
	if (bIsBoundary)
	{
		return m_pModelCom->Get_IsEndAnimArray_Boundary()[iAnimIndex];
	}
	else
	{
		return m_pModelCom->Get_IsEndAnimArray()[iAnimIndex];
	}

}

void CPawn::Play_Sound(const PAWN_SOUND_TYPE eType, const TCHAR* pSoundKey, _bool isLoop)
{
	_float* fVolume = {};
	switch (eType)
	{
		case PAWN_SOUND_VOICE:
			fVolume = &g_fVoiceVolume;
			break;
		case PAWN_SOUND_EFFECT1:
		case PAWN_SOUND_EFFECT2:
			fVolume = &g_fEffectVolume;
			break;
	default:
		return;
	}
	m_pSoundCom[eType]->Play2D(pSoundKey, fVolume, isLoop);
}

void CPawn::PlayRepeat_Sound(const PAWN_SOUND_TYPE eType, const TCHAR* pSoundKey)
{
	_float* fVolume = {};
	switch (eType)
	{
	case PAWN_SOUND_VOICE:
		fVolume = &g_fVoiceVolume;
		break;
	case PAWN_SOUND_EFFECT1:
	case PAWN_SOUND_EFFECT2:
		fVolume = &g_fEffectVolume;
		break;
	default:
		return;
	}
	m_pSoundCom[eType]->Play2D_Repeat(pSoundKey, fVolume);
}

void CPawn::Stop_Sound(const PAWN_SOUND_TYPE eType)
{
	m_pSoundCom[eType]->Stop();
}

_int CPawn::Get_UFBIndex(_uint UFB_Type)
{
	return m_pModelCom->Get_UFBIndices(UFB_Type);
}

_Vec4 CPawn::Calc_CenterPos(_bool isUsingBone)
{
	if (nullptr == m_pModelCom || !m_pModelCom->Get_IsUseRootBone())
		return m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_vCenterOffset;
	
	_Vec4 vPos{};
	
	if (isUsingBone)
	{
		vPos = m_pModelCom->Calc_CenterPos(m_pTransformCom->Get_WorldMatrix()) + m_vCenterOffset;
	}
	else
	{
		vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_vCenterOffset;
	}
	
	return vPos;
}

_Vec3 CPawn::Calc_BoneWorldPos(const _char* szBoneName)
{
	if (nullptr == m_pModelCom)
		return (_Vec3)m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_Matrix WorldMatrix;
	_matrix SocketMatrix = m_pModelCom->Get_BoneCombindTransformationMatrix(szBoneName);

	for (size_t i = 0; i < 3; i++)
	{
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
	}
	XMStoreFloat4x4(&WorldMatrix, SocketMatrix * XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));

	return WorldMatrix.Translation();
}

HRESULT CPawn::Bind_WorldViewProj()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPawn::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NaviDesc{};
	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	/* FOR.Com_VoiceSound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_VoiceSound"), reinterpret_cast<CComponent**>(&m_pSoundCom[PAWN_SOUND_VOICE]))))
		return E_FAIL;
	m_pSoundCom[PAWN_SOUND_VOICE]->Set_Owner(this);

	/* FOR.Com_EffectSound */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_EffectSound"), reinterpret_cast<CComponent**>(&m_pSoundCom[PAWN_SOUND_EFFECT1]))))
		return E_FAIL;
	m_pSoundCom[PAWN_SOUND_EFFECT1]->Set_Owner(this);

	/* FOR.Com_EffectSound2 */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Sound"),
		TEXT("Com_EffectSound2"), reinterpret_cast<CComponent**>(&m_pSoundCom[PAWN_SOUND_EFFECT2]))))
		return E_FAIL;
	m_pSoundCom[PAWN_SOUND_EFFECT2]->Set_Owner(this);


	return S_OK;
}

void CPawn::Free()
{
	__super::Free();

	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pColliderCom);

	if (m_pFsmCom != nullptr)
	{
		m_pFsmCom->Release_States();
	}
	Safe_Release(m_pFsmCom);

	for (_uint i = 0; i < PAWN_SOUND_END; ++i)
	{
		Safe_Release(m_pSoundCom[i]);
	}
}
