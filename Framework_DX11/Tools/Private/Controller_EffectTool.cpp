#include "stdafx.h"
#include "ImGui_Manager.h"

#include "Controller_EffectTool.h"
#include "GameInstance.h"
#include "Layer.h"

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

	CParticle_Test::DEFAULT_DESC DefaultDesc = {};
	DefaultDesc.eType = (CParticle_Test::TYPE)m_iParticleType;
	DefaultDesc.iState = m_iParticleState;
	DefaultDesc.fRenderRatio = m_fRandomRatio;
	DefaultDesc.vPivot = m_vPivot;
	DefaultDesc.fGravity = m_fGravity;
	DefaultDesc.vMoveDir = m_vMoveDir;

	CParticle_Test::REVOLVE_DESC RevolveDesc = {};
	RevolveDesc.vRevolveAxis = m_vRevolveAxis;
	RevolveDesc.fAngle = m_fAngle;

	CParticle_Test::RANDOM_DESC RandomDesc = {};
	RandomDesc.fTimeInterval = m_fTimeInterval;
	RandomDesc.fRandomRatio = m_fRandomRatio;

	CParticle_Test::ACCEL_DESC AccelDesc = {};
	AccelDesc.fAccelLimit = m_fAccelLimit;
	AccelDesc.fAccelSpeed = m_fAccelSpeed;

	CParticle_Test::TRANSFORM_DESC TransformDesc = {};
	TransformDesc.vPos = m_vPos;
	TransformDesc.vRotation = m_vRotation;
	TransformDesc.vScale = m_vScale;

	desc.DefaultDesc = DefaultDesc;
	desc.RevolveDesc = RevolveDesc;
	desc.RandomDesc = RandomDesc;
	desc.AccelDesc = AccelDesc;
	desc.TransformDesc = TransformDesc;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Test"), &desc)))
		return E_FAIL;

	++m_iParticleCount;

	return S_OK;
}

void CController_EffectTool::Check()
{
	if (ImGui::TreeNode("Initialize"))
	{
		ImGui::SeparatorText("Initialize");
		ImGui::InputInt	("Num Instance", (_int*)&m_iNumInstance);
		ImGui::InputFloat3("Center Instance", (_float*)&m_vCenter);
		ImGui::InputFloat3("Range Instance", (_float*)&m_vRange);
		ImGui::InputFloat3("ExceptRange Instance", (_float*)&m_vExceptRange);
		ImGui::InputFloat2("Size Instance", (_float*)&m_vSize);
		ImGui::InputFloat2("Speed Instance", (_float*)&m_vSpeed);
		ImGui::InputFloat2("LifeTime Instance", (_float*)&m_vLifeTime);
		ImGui::InputFloat4("MinColor Instance", (_float*)&m_vMinColor);
		ImGui::InputFloat4("MaxColor Instance", (_float*)&m_vMaxColor);
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

		ImGui::Checkbox("Accel", &m_bAccel);
		ImGui::SameLine();
		ImGui::Checkbox("Decel", &m_bDecel);
		
		Set_State();
		
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
		ImGui::InputFloat("Gravity", (_float*)&m_fGravity);
		ImGui::InputFloat4("Move Dir", (_float*)&m_vMoveDir);

		ImGui::SeparatorText("Particle Revolve"); 
		ImGui::InputFloat3("Revolve Axis", (_float*)&m_vRevolveAxis);
		ImGui::InputFloat("Angle", (_float*)&m_fAngle);

		ImGui::SeparatorText("Particle Random");
		ImGui::InputFloat("Time Interval", (_float*)&m_fTimeInterval);
		ImGui::InputFloat("Random Ratio", (_float*)&m_fRandomRatio);

		ImGui::SeparatorText("Particle Accel");
		ImGui::InputFloat("Accel Speed", (_float*)&m_fAccelSpeed);
		ImGui::InputFloat("Accel Limit", (_float*)&m_fAccelLimit);

		ImGui::TreePop();
	}
}

void CController_EffectTool::Update_Particle()
{
	CParticle_Test* pParticle = static_cast<CParticle_Test*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_Particle"), m_iSelectedParticleIndex));

	if (nullptr == pParticle)
		return;

	CParticle_Test::DEFAULT_DESC DefaultDesc = {};
	DefaultDesc.eType = (CParticle_Test::TYPE)m_iParticleType;
	DefaultDesc.iState = m_iParticleState;
	DefaultDesc.fRenderRatio = m_fRenderRatio;
	DefaultDesc.vPivot = m_vPivot;
	DefaultDesc.fGravity = m_fGravity;
	DefaultDesc.vMoveDir = m_vMoveDir;

	pParticle->Set_Default(DefaultDesc);

	CParticle_Test::REVOLVE_DESC RevolveDesc = {};
	RevolveDesc.vRevolveAxis = m_vRevolveAxis;
	RevolveDesc.fAngle = m_fAngle;
	
	pParticle->Set_Revolev(RevolveDesc);

	CParticle_Test::RANDOM_DESC RandomDesc = {};
	RandomDesc.fTimeInterval = m_fTimeInterval;
	RandomDesc.fRandomRatio = m_fRandomRatio;

	pParticle->Set_Random(RandomDesc);

	CParticle_Test::ACCEL_DESC AccelDesc = {};
	AccelDesc.fAccelLimit = m_fAccelLimit;
	AccelDesc.fAccelSpeed = m_fAccelSpeed;

	pParticle->Set_Accel(AccelDesc);

	CParticle_Test::TRANSFORM_DESC TransformDesc = {};
	TransformDesc.vPos = m_vPos;
	TransformDesc.vRotation = m_vRotation;
	TransformDesc.vScale = m_vScale;

	pParticle->Set_Transform(TransformDesc);
}

void CController_EffectTool::Select_Particle()
{
	vector<string> IndexLavels;
	vector<const _char*> Items;

	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Particle"));
	
	if (nullptr != pLayer)	
	{
		_uint iIndex = { 0 };
		for (const auto& elem : pLayer->Get_ObjectList())
		{
			IndexLavels.emplace_back("Index " + to_string(iIndex));
			++iIndex;
		}

		for (const auto& Label : IndexLavels)
			Items.emplace_back(Label.c_str());
	}

	ImGui::ListBox("Particle List", &m_iSelectedParticleIndex, Items.data(), (_int)Items.size(), 5);
}

void CController_EffectTool::Delete_Particle()
{
	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Particle"));

	if (nullptr == pLayer)
		return;

	auto& ObjectList = pLayer->Get_ObjectList();
	_uint iIndex = { 0 };
	for (auto& iter = ObjectList.begin(); iter != ObjectList.end();)
	{
		if (iIndex == m_iSelectedParticleIndex)
		{
			Safe_Release(*iter);
			iter = ObjectList.erase(iter);
			return;
		}
		else
			++iter;

		++iIndex;
	}

}

void CController_EffectTool::Get_Particle()
{
	CParticle_Test* pParticle = static_cast<CParticle_Test*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_Particle"), m_iSelectedParticleIndex));

	CParticle_Test::DEFAULT_DESC DefaultDesc = pParticle->Get_Default();
	CParticle_Test::REVOLVE_DESC RevolveDesc = pParticle->Get_Revolve();
	CParticle_Test::RANDOM_DESC RandomDesc = pParticle->Get_Random();
	CParticle_Test::ACCEL_DESC AccelDesc = pParticle->Get_Accel();

	m_iParticleType = DefaultDesc.eType;
	m_iParticleState = DefaultDesc.iState;
	m_fRenderRatio = DefaultDesc.fRenderRatio;
	m_vPivot = DefaultDesc.vPivot;
	m_fGravity = DefaultDesc.fGravity;
	m_vMoveDir = DefaultDesc.vMoveDir;

	m_vRevolveAxis = RevolveDesc.vRevolveAxis;
	m_fAngle = RevolveDesc.fAngle;

	m_fTimeInterval = RandomDesc.fTimeInterval;
	m_fRandomRatio = RandomDesc.fRandomRatio;

	m_fAccelLimit = AccelDesc.fAccelLimit;
	m_fAccelSpeed = AccelDesc.fAccelSpeed;

	if (CVIBuffer_Point_Instance::STATE_REVOLVE == (m_iParticleState & CVIBuffer_Point_Instance::STATE_REVOLVE))
		m_bRevolve = true;
	else
		m_bRevolve = false;

	if (CVIBuffer_Point_Instance::STATE_RANDOM == (m_iParticleState & CVIBuffer_Point_Instance::STATE_RANDOM))
		m_bRandom = true;
	else
		m_bRandom = false;

	if (CVIBuffer_Point_Instance::STATE_LOOP == (m_iParticleState & CVIBuffer_Point_Instance::STATE_LOOP))
		m_bLoop = true;
	else
		m_bLoop = false;

	if (CVIBuffer_Point_Instance::STATE_ACCEL == (m_iParticleState & CVIBuffer_Point_Instance::STATE_ACCEL))
		m_bAccel = true;
	else
		m_bAccel = false;

	if (CVIBuffer_Point_Instance::STATE_DECEL == (m_iParticleState & CVIBuffer_Point_Instance::STATE_DECEL))
		m_bDecel = true;
	else
		m_bDecel = false;

}

void CController_EffectTool::Set_State()
{
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

	if (true == m_bAccel)
		m_iParticleState |= CVIBuffer_Point_Instance::STATE_ACCEL;
	else
		m_iParticleState &= ~CVIBuffer_Point_Instance::STATE_ACCEL;

	if (true == m_bDecel)
		m_iParticleState |= CVIBuffer_Point_Instance::STATE_DECEL;
	else
		m_iParticleState &= ~CVIBuffer_Point_Instance::STATE_DECEL;
}

void CController_EffectTool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
