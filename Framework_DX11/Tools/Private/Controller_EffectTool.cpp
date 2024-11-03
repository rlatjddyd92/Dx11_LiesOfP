#include "stdafx.h"
#include "ImGui_Manager.h"

#include "Controller_EffectTool.h"
#include "GameInstance.h"

#include "Particle_Test.h"

IMPLEMENT_SINGLETON(CController_EffectTool)

CController_EffectTool::CController_EffectTool()
	:m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CController_EffectTool::Initialize()
{
	
	return S_OK;
}

HRESULT CController_EffectTool::Add_Particle()
{
	CParticle_Test::PARTICLE_TEST_DESC desc = {};
	desc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.fSpeedPerSec = 1.f;
	desc.iLevelIndex = LEVEL_TOOL;
	desc.iNumInstance = m_iNumInstance;
	desc.vCenter = m_vCenter;
	desc.vExceptRange = m_vExceptRange;
	desc.vLifeTime = m_vLifeTime;
	desc.vMaxColor = m_vMaxColor;
	desc.vMinColor = m_vMinColor;
	desc.vRange = m_vRange;
	desc.vSize = m_vSize;
	desc.vSpeed = m_vSpeed;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle"), &desc)))
		return E_FAIL;

	++m_iParticleCount;

	return S_OK;
}

void CController_EffectTool::Check()
{
	if (ImGui::TreeNode("Initialize"))
	{
		ImGui::SeparatorText("Initialize");
		ImGui::InputInt("Instance Num", (_int*)&m_iNumInstance);
		ImGui::InputFloat3("Instance Center", (_float*)&m_vCenter);
		ImGui::InputFloat3("Instance Range", (_float*)&m_vRange);
		ImGui::InputFloat3("Instance ExceptRange", (_float*)&m_vExceptRange);
		ImGui::InputFloat2("Instance Size", (_float*)&m_vSize);
		ImGui::InputFloat2("Instance Speed", (_float*)&m_vSpeed);
		ImGui::InputFloat2("Instance LifeTime", (_float*)&m_vLifeTime);
		ImGui::InputFloat4("Instance MinColor", (_float*)&m_vMinColor);
		ImGui::InputFloat4("Instance MaxColor", (_float*)&m_vMaxColor);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Type/State"))
	{
		ImGui::SeparatorText("Normal");
		ImGui::RadioButton("Spread", (_int*)&m_iParticleType, CParticle_Test::TYPE_SPREAD);
		ImGui::SameLine();
		ImGui::RadioButton("Move", (_int*)&m_iParticleType, CParticle_Test::TYPE_MOVE);
		ImGui::SameLine();
		ImGui::RadioButton("Converge", (_int*)&m_iParticleType, CParticle_Test::TYPE_CONVERGE);

		ImGui::SeparatorText("Indepedent");
		ImGui::RadioButton("Spread_I", (_int*)&m_iParticleType, CParticle_Test::TYPE_SPREAD_INDEPENDENT);
		ImGui::SameLine();
		ImGui::RadioButton("Move_I", (_int*)&m_iParticleType, CParticle_Test::TYPE_MOVE_INDEPENDENT);
		ImGui::SameLine();
		ImGui::RadioButton("Converge_I", (_int*)&m_iParticleType, CParticle_Test::TYPE_CONVERGE_INDEPENDENT);

		ImGui::SeparatorText("State");
		ImGui::Checkbox("Revolve", &m_bRevolve);
		ImGui::SameLine();
		ImGui::Checkbox("Random", &m_bRandom);
		ImGui::SameLine();
		ImGui::Checkbox("Loop", &m_bLoop);

		if (true == m_bRevolve)
			m_iParticleState |= CVIBuffer_Point_Instance::STATE_REVOLVE;
		else
			m_iParticleState &= ~CVIBuffer_Point_Instance::STATE_REVOLVE;

		if (true == m_bRandom)
			m_iParticleState |= CVIBuffer_Point_Instance::STATE_RANDOM;
		else
			m_iParticleState &= ~CVIBuffer_Point_Instance::STATE_RANDOM;

		if (true == m_bLoop)
			m_iParticleState |= CVIBuffer_Point_Instance::STATE_LOOP;
		else
			m_iParticleState &= ~CVIBuffer_Point_Instance::STATE_LOOP;

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Additional"))
	{
		ImGui::SeparatorText("Transform");
		ImGui::InputFloat3("Pos", (_float*)&m_vPos);
		ImGui::InputFloat3("Rotation", (_float*)&m_vRotation);
		ImGui::InputFloat3("Scale", (_float*)&m_vScale);

		ImGui::SeparatorText("Particle Action"); 
		ImGui::InputFloat("Render Ratio", (_float*)&m_fRenderRatio);
		ImGui::InputFloat4("Pivot", (_float*)&m_vPivot);
		ImGui::InputFloat("Speed", (_float*)&m_fSpeed);
		ImGui::InputFloat("Gravity", (_float*)&m_fGravity);
		ImGui::InputFloat4("Move Dir", (_float*)&m_vMoveDir);

		ImGui::SeparatorText("Particle Revolve"); 
		ImGui::InputFloat3("Revolve Axis", (_float*)&m_vRevolveAxis);
		ImGui::InputFloat("Angle", (_float*)&m_fAngle);

		ImGui::SeparatorText("Particle Random");
		ImGui::InputFloat("Time Interval", (_float*)&m_fTimeInterval);
		ImGui::InputFloat("Random Ratio", (_float*)&m_fRandomRatio);

		ImGui::TreePop();
	}
}

void CController_EffectTool::Update_Particle()
{
	CParticle_Test* pParticle = static_cast<CParticle_Test*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_Particle"), m_iParticleCount - 1));

	if (nullptr == pParticle)
		return;

	pParticle->Set_Type((CParticle_Test::TYPE)m_iParticleType);
	pParticle->Set_State(m_iParticleState);
	pParticle->Set_RenderRatio(m_fRenderRatio);
	pParticle->Set_Pivot(m_vPivot);
	pParticle->Set_Speed(m_fSpeed);
	pParticle->Set_Gravity(m_fGravity);
	pParticle->Set_MoveDir(m_vMoveDir);
	pParticle->Set_RevolveAxis(m_vRevolveAxis);
	pParticle->Set_Angle(m_fAngle);
	pParticle->Set_TimeInterval(m_fTimeInterval);
	pParticle->Set_RandomRatio(m_fRandomRatio);
}

void CController_EffectTool::Select_Effect()
{
	
}

void CController_EffectTool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
