#include "Pawn.h"
#include "GameInstance.h"
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

	return S_OK;

#ifdef _DEBUG
	m_pColliderCom->Render();
	m_pNavigationCom->Render();
#endif

	return S_OK;
}

_bool CPawn::Calc_DamageGain(_float fAtkDmg)
{

	//if () 피해를 입는지에관한 판단, 무적인지등
	//{
	//
	//}		//피해를 입는다면 true 반환, 
	
	m_eStat.fHp -= (fAtkDmg * ((100 - m_eStat.fDefence) / 100));
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

void CPawn::Change_Animation(_uint iAnimIndex, _bool IsLoop, _float fDuration, _uint iStartFrame, _bool bEitherBoundary)
{
	m_pModelCom->SetUp_NextAnimation(iAnimIndex, IsLoop, fDuration, iStartFrame, bEitherBoundary);
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

_int CPawn::Get_Frame()
{
	return m_pModelCom->Get_CurrentFrame();
}

_double CPawn::Get_CurrentTrackPos()
{
	return m_pModelCom->Get_CurrentTrackPosition();
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

}
