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
	return S_OK;
}

_bool CPawn::Calc_DamageGain(_float fAtkDmg)
{

	//if () 피해를 입는지에관한 판단, 무적인지등
	//{
	//
	//}		//피해를 입는다면 true 반환, 
	
	m_fHp -= (fAtkDmg * ((100 - m_fDefence) / 100));

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
