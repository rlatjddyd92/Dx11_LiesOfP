#include "stdafx.h"
#include "ImGui_Manager.h"

#include "Controller_EffectTool.h"
#include "GameInstance.h"
#include "Layer.h"

#include "Particle_Effect.h"
#include "Texture_Effect.h"

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
	CParticle_Effect::PARTICLE_TEST_DESC desc = {};
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

	CParticle_Effect::DEFAULT_DESC DefaultDesc = {};
	DefaultDesc.eType = (CParticle_Effect::TYPE)m_iParticleType;
	DefaultDesc.iState = m_iParticleState;
	DefaultDesc.fRenderRatio = m_fRandomRatio;
	DefaultDesc.vPivot = m_vPivot;
	DefaultDesc.fGravity = m_fGravity;
	DefaultDesc.vMoveDir = m_vMoveDir;

	CParticle_Effect::REVOLVE_DESC OrbitDesc = {};
	OrbitDesc.vOrbitAxis = m_vOrbitAxis;
	OrbitDesc.fAngle = m_fAngle;

	CParticle_Effect::RANDOM_DESC RandomDesc = {};
	RandomDesc.fTimeInterval = m_fTimeInterval;
	RandomDesc.fRandomRatio = m_fRandomRatio;

	CParticle_Effect::ACCEL_DESC AccelDesc = {};
	AccelDesc.fAccelLimit = m_fAccelLimit;
	AccelDesc.fAccelSpeed = m_fAccelSpeed;

	CParticle_Effect::TRANSFORM_DESC TransformDesc = {};
	TransformDesc.vPos = m_vParticlePos;
	TransformDesc.vRotation = m_vParticleRotation;
	TransformDesc.vScale = m_vParticleScale;

	desc.DefaultDesc = DefaultDesc;
	desc.OrbitDesc = OrbitDesc;
	desc.RandomDesc = RandomDesc;
	desc.AccelDesc = AccelDesc;
	desc.TransformDesc = TransformDesc;

	desc.strPrototypeTextureTag = m_Particle_PrototypeTags[m_iSelectedParticleTextureIndex];

	if (m_iMax_ParticleShaderIndex < m_iParticleShaderIndex)
		m_iParticleShaderIndex = m_iMax_ParticleShaderIndex;

	desc.iShaderIndex = m_iParticleShaderIndex;
	desc.iRenderState = m_iParticleRenderState;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Effect"), &desc)))
		return E_FAIL;

	return S_OK;
}

void CController_EffectTool::Particle_Check()
{
	if (ImGui::TreeNode("Particle Texture"))
	{
		vector<string> strArray;

		for (const auto& strPrototype : m_Particle_PrototypeTags)
		{
			int iSize = WideCharToMultiByte(CP_UTF8, 0, &strPrototype[0], (int)strPrototype.size(), nullptr, 0, nullptr, nullptr);
			string strTo(iSize, 0);
			WideCharToMultiByte(CP_UTF8, 0, &strPrototype[0], (int)strPrototype.size(), &strTo[0], iSize, nullptr, nullptr);
			strArray.emplace_back(strTo);
		}

		vector<const _char*> szItems;
		for (const auto& str : strArray)
		{
			szItems.emplace_back(str.c_str());
		}

		ImGui::ListBox("Particle Texture List", &m_iSelectedParticleTextureIndex, szItems.data(), (_int)szItems.size(), 5);

		ImGui::SeparatorText("Shader");
		ImGui::InputInt("Shader Index", (_int*)&m_iParticleShaderIndex);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Particle Initialize"))
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
	if (ImGui::TreeNode("Particle Type/State"))
	{
		ImGui::SeparatorText("Normal");
		ImGui::RadioButton("Spread", (_int*)&m_iParticleType, CParticle_Effect::TYPE_SPREAD);
		ImGui::SameLine();
		ImGui::RadioButton("Move", (_int*)&m_iParticleType, CParticle_Effect::TYPE_MOVE);
		ImGui::SameLine();
		ImGui::RadioButton("Converge", (_int*)&m_iParticleType, CParticle_Effect::TYPE_CONVERGE);

		ImGui::SeparatorText("Indepedent");
		ImGui::RadioButton("Spread_I", (_int*)&m_iParticleType, CParticle_Effect::TYPE_SPREAD_INDEPENDENT);
		ImGui::SameLine();
		ImGui::RadioButton("Move_I", (_int*)&m_iParticleType, CParticle_Effect::TYPE_MOVE_INDEPENDENT);
		ImGui::SameLine();
		ImGui::RadioButton("Converge_I", (_int*)&m_iParticleType, CParticle_Effect::TYPE_CONVERGE_INDEPENDENT);

		ImGui::SeparatorText("State");
		ImGui::Checkbox("Orbit", &m_bOrbit);
		ImGui::SameLine();
		ImGui::Checkbox("Random", &m_bRandom);
		ImGui::SameLine();
		ImGui::Checkbox("Loop", &m_bLoop);

		ImGui::Checkbox("Accel", &m_bAccel);
		ImGui::SameLine();
		ImGui::Checkbox("Decel", &m_bDecel);
		ImGui::SameLine();
		ImGui::Checkbox("Blur", &m_bBlur);

		Set_ParticleState();
		
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Particle Additional"))
	{
		ImGui::SeparatorText("Transform");
		ImGui::InputFloat3("Pos", (_float*)&m_vParticlePos);
		ImGui::InputFloat3("Rotation", (_float*)&m_vParticleRotation);
		ImGui::InputFloat3("Scale", (_float*)&m_vParticleScale);

		ImGui::SeparatorText("Particle Action"); 
		ImGui::InputFloat("Render Ratio", (_float*)&m_fRenderRatio);
		ImGui::InputFloat4("Pivot", (_float*)&m_vPivot);
		ImGui::InputFloat("Gravity", (_float*)&m_fGravity);
		ImGui::InputFloat4("Move Dir", (_float*)&m_vMoveDir);

		ImGui::SeparatorText("Particle Orbit");
		ImGui::InputFloat3("Orbit Axis", (_float*)&m_vOrbitAxis);
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
	CParticle_Effect* pParticle = static_cast<CParticle_Effect*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_Particle"), m_iSelectedParticleIndex));

	if (nullptr == pParticle)
		return;

	CParticle_Effect::DEFAULT_DESC DefaultDesc = {};
	DefaultDesc.eType = (CParticle_Effect::TYPE)m_iParticleType;
	DefaultDesc.iState = m_iParticleState;
	DefaultDesc.fRenderRatio = m_fRenderRatio;
	DefaultDesc.vPivot = m_vPivot;
	DefaultDesc.fGravity = m_fGravity;
	DefaultDesc.vMoveDir = m_vMoveDir;

	pParticle->Set_Default(DefaultDesc);

	CParticle_Effect::REVOLVE_DESC OrbitDesc = {};
	OrbitDesc.vOrbitAxis = m_vOrbitAxis;
	OrbitDesc.fAngle = m_fAngle;
	
	pParticle->Set_Revolev(OrbitDesc);

	CParticle_Effect::RANDOM_DESC RandomDesc = {};
	RandomDesc.fTimeInterval = m_fTimeInterval;
	RandomDesc.fRandomRatio = m_fRandomRatio;

	pParticle->Set_Random(RandomDesc);

	CParticle_Effect::ACCEL_DESC AccelDesc = {};
	AccelDesc.fAccelLimit = m_fAccelLimit;
	AccelDesc.fAccelSpeed = m_fAccelSpeed;

	pParticle->Set_Accel(AccelDesc);

	CParticle_Effect::TRANSFORM_DESC TransformDesc = {};
	TransformDesc.vPos = m_vParticlePos;
	TransformDesc.vRotation = m_vParticleRotation;
	TransformDesc.vScale = m_vParticleScale;

	pParticle->Set_Transform(TransformDesc);

	if (m_iMax_ParticleShaderIndex < m_iParticleShaderIndex)
		m_iParticleShaderIndex = m_iMax_ParticleShaderIndex;

	pParticle->Set_ShaderIndex(m_iParticleShaderIndex);

	pParticle->Set_RenderState(m_iParticleRenderState);
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

void CController_EffectTool::Get_Particle()
{
	CParticle_Effect* pParticle = static_cast<CParticle_Effect*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_Particle"), m_iSelectedParticleIndex));

	if (nullptr == pParticle)
		return;

	CParticle_Effect::DEFAULT_DESC DefaultDesc = pParticle->Get_Default();
	CParticle_Effect::REVOLVE_DESC OrbitDesc = pParticle->Get_Orbit();
	CParticle_Effect::RANDOM_DESC RandomDesc = pParticle->Get_Random();
	CParticle_Effect::ACCEL_DESC AccelDesc = pParticle->Get_Accel();

	m_iParticleType = DefaultDesc.eType;
	m_iParticleState = DefaultDesc.iState;
	m_fRenderRatio = DefaultDesc.fRenderRatio;
	m_vPivot = DefaultDesc.vPivot;
	m_fGravity = DefaultDesc.fGravity;
	m_vMoveDir = DefaultDesc.vMoveDir;

	m_vOrbitAxis = OrbitDesc.vOrbitAxis;
	m_fAngle = OrbitDesc.fAngle;

	m_fTimeInterval = RandomDesc.fTimeInterval;
	m_fRandomRatio = RandomDesc.fRandomRatio;

	m_fAccelLimit = AccelDesc.fAccelLimit;
	m_fAccelSpeed = AccelDesc.fAccelSpeed;

	m_iParticleRenderState = pParticle->Get_RenderState();

	if (CVIBuffer_Point_Instance::STATE_ORBIT == (m_iParticleState & CVIBuffer_Point_Instance::STATE_ORBIT))
		m_bOrbit = true;
	else
		m_bOrbit = false;

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

	if (CParticle_Effect::RS_BLUR == (m_iParticleRenderState & CParticle_Effect::RS_BLUR))
		m_bBlur = true;
	else
		m_bBlur = false;
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

HRESULT CController_EffectTool::Add_TE()
{
	CTexture_Effect::TEXTURE_EFFECT_DESC desc = {};

	desc.fRotationPerSec	= XMConvertToRadians(90.f);
	desc.fSpeedPerSec		= 1.f;
	desc.iLevelIndex		= LEVEL_TOOL;
	
	desc.strTexturTag		= m_TE_PrototypeTags[m_iSelectedTETextureIndex];
	
	desc.iState				= m_iTEState;
	desc.iShaderIndex		= m_iTEShaderIndex;
	desc.fDuration			= m_fDuration;
	desc.vColor				= m_vColor;
	desc.vDivide			= m_vDivide;
	desc.fSpriteSpeed		= m_fSpriteSpeed;

	desc.vPos				= m_vTEPos;
	
	desc.fRotationAngle		= m_fRotationAngle;
	desc.fRotationSpeed		= m_fRotationSpeed;
	
	desc.vScale				= m_vTEScale;
	desc.fScalingSpeed		= m_fScalingSpeed;
	
	desc.fAlpha				= m_fAlpha;
	desc.fAlphaSpeed		= m_fAlphaSpeed;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_TextureEffect"), TEXT("Prototype_GameObject_Texture_Effect"), &desc)))
		return E_FAIL;

	return S_OK;
}

void CController_EffectTool::TE_Check()
{
	if (ImGui::TreeNode("TE Texture"))
	{
		vector<string> strArray;

		for (const auto& strPrototype : m_TE_PrototypeTags)
		{
			int iSize = WideCharToMultiByte(CP_UTF8, 0, &strPrototype[0], (int)strPrototype.size(), nullptr, 0, nullptr, nullptr);
			string strTo(iSize, 0);
			WideCharToMultiByte(CP_UTF8, 0, &strPrototype[0], (int)strPrototype.size(), &strTo[0], iSize, nullptr, nullptr);
			strArray.emplace_back(strTo);
		}

		vector<const _char*> szItems;
		for (const auto& str : strArray)
		{
			szItems.emplace_back(str.c_str());
		}

		ImGui::ListBox("TE Texture List", &m_iSelectedTETextureIndex, szItems.data(), (_int)szItems.size(), 5);

		ImGui::SeparatorText("Shader");
		ImGui::InputInt("Shader Index", (_int*)&m_iTEShaderIndex);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("TE Initialize"))
	{
		ImGui::SeparatorText("Initialize");
		ImGui::InputFloat("Duration TE", (_float*)&m_fDuration);
		ImGui::InputFloat4("Color TE", (_float*)&m_vColor);
		ImGui::InputFloat2("Divide TE", (_float*)&m_vDivide);
		ImGui::InputFloat("Sprite Speed TE", (_float*)&m_fSpriteSpeed);

		ImGui::SeparatorText("Transform");
		ImGui::InputFloat3("Pos TE", (_float*)&m_vTEPos);
		ImGui::InputFloat("Rotation Angle TE", (_float*)&m_fRotationAngle);
		ImGui::InputFloat("Rotation Speed TE", (_float*)&m_fRotationSpeed);
		ImGui::InputFloat3("Scale TE", (_float*)&m_vTEScale);
		ImGui::InputFloat("Scaling Speed TE", (_float*)&m_fScalingSpeed);

		ImGui::SeparatorText("Blending");
		ImGui::InputFloat("Alpha TE", (_float*)&m_fAlpha);
		ImGui::InputFloat("Alpha Speed TE", (_float*)&m_fAlphaSpeed);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("TE Type/State"))
	{
		ImGui::SeparatorText("State");
		ImGui::Checkbox("Scaling TE", &m_bScaling);
		ImGui::Checkbox("Turn TE", &m_bTurn);
		ImGui::Checkbox("FadeIn TE", &m_bFadeIn);
		ImGui::SameLine();
		ImGui::Checkbox("FadeOut TE", &m_bFadeOut);
		ImGui::Checkbox("Loop TE", &m_isLoop);
		ImGui::Checkbox("Distortion TE", &m_isDistortion);
		ImGui::SameLine();
		ImGui::Checkbox("Blur TE", &m_isBlur);

		Set_TEState();

		ImGui::TreePop();
	}

}

void CController_EffectTool::Update_TE()
{
	CTexture_Effect* pTE = static_cast<CTexture_Effect*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_TextureEffect"), m_iSelectedTEIndex));

	if (nullptr == pTE)
		return;

	CTexture_Effect::TEXTURE_EFFECT_DESC desc = {};

	desc.iState = m_iTEState;
	desc.iShaderIndex = m_iTEShaderIndex;
	desc.fDuration = m_fDuration;
	desc.vColor = m_vColor;
	desc.vPos = m_vTEPos;
	desc.fRotationAngle = m_fRotationAngle;
	desc.fRotationSpeed = m_fRotationSpeed;
	desc.vScale = m_vTEScale;
	desc.fScalingSpeed = m_fScalingSpeed;
	desc.fAlpha = m_fAlpha;
	desc.fAlphaSpeed = m_fAlphaSpeed;
	desc.vDivide = m_vDivide;
	desc.fSpriteSpeed = m_fSpriteSpeed;

	pTE->Set_Desc(desc);
}

void CController_EffectTool::Select_TE()
{
	vector<string> IndexLavels;
	vector<const _char*> Items;

	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_TextureEffect"));

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

	ImGui::ListBox("TE List", &m_iSelectedTEIndex, Items.data(), (_int)Items.size(), 5);

}

void CController_EffectTool::Get_TE()
{
	CTexture_Effect* pTE = static_cast<CTexture_Effect*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_TextureEffect"), m_iSelectedTEIndex));

	if (nullptr == pTE)
		return;

	CTexture_Effect::TEXTURE_EFFECT_DESC desc = pTE->Get_Desc();

	m_iTEState = desc.iState;
	m_iTEShaderIndex = desc.iShaderIndex;
	m_fDuration = desc.fDuration;
	m_vColor = desc.vColor;
	m_fRotationSpeed = desc.fRotationSpeed;
	m_fScalingSpeed = desc.fScalingSpeed;
	m_fAlpha = desc.fAlpha;
	m_fAlphaSpeed = desc.fAlphaSpeed;
	m_vDivide = desc.vDivide;
	m_fSpriteSpeed = desc.fSpriteSpeed;

	//m_vTEPos = desc.vPos;
	//m_vTEScale = desc.vScale;
	//m_fRotationAngle = desc.fRotationAngle;
	
	if (m_iTEState & CTexture_Effect::STATE_SCALING)
		m_bScaling = true;
	else
		m_bScaling = false;

	if (m_iTEState & CTexture_Effect::STATE_ROTATION)
		m_bTurn = true;
	else
		m_bTurn = false;

	if (m_iTEState & CTexture_Effect::STATE_FADEIN)
		m_bFadeIn = true;
	else
		m_bFadeIn = false;

	if (m_iTEState & CTexture_Effect::STATE_FADEOUT)
		m_bFadeOut = true;
	else
		m_bFadeOut = false;

	if (m_iTEState & CTexture_Effect::STATE_LOOP)
		m_isLoop = true;
	else
		m_isLoop = false;

	if (m_iTEState & CTexture_Effect::STATE_DISTORTION)
		m_isDistortion = true;
	else
		m_isDistortion = false;

	if (m_iTEState & CTexture_Effect::STATE_BLUR)
		m_isBlur = true;
	else
		m_isBlur = false;

}

void CController_EffectTool::Delete_TE()
{
	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_TextureEffect"));

	if (nullptr == pLayer)
		return;

	auto& ObjectList = pLayer->Get_ObjectList();
	_uint iIndex = { 0 };
	for (auto& iter = ObjectList.begin(); iter != ObjectList.end();)
	{
		if (iIndex == m_iSelectedTEIndex)
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


void CController_EffectTool::Set_ParticleState()
{
	if (true == m_bOrbit)
		m_iParticleState |= CVIBuffer_Point_Instance::STATE_ORBIT;
	else
		m_iParticleState &= ~CVIBuffer_Point_Instance::STATE_ORBIT;

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

	if (true == m_bBlur)
		m_iParticleRenderState |= CParticle_Effect::RS_BLUR;
	else
		m_iParticleRenderState &= ~CParticle_Effect::RS_BLUR;
}

void CController_EffectTool::Set_TEState()
{
	if (true == m_bScaling)
		m_iTEState |= CTexture_Effect::STATE_SCALING;
	else
		m_iTEState &= ~CTexture_Effect::STATE_SCALING;
	
	if (true == m_bTurn)
		m_iTEState |= CTexture_Effect::STATE_ROTATION;
	else
		m_iTEState &= ~CTexture_Effect::STATE_ROTATION;

	if (true == m_bFadeIn)
		m_iTEState |= CTexture_Effect::STATE_FADEIN;
	else
		m_iTEState &= ~CTexture_Effect::STATE_FADEIN;

	if (true == m_bFadeOut)
		m_iTEState |= CTexture_Effect::STATE_FADEOUT;
	else
		m_iTEState &= ~CTexture_Effect::STATE_FADEOUT;

	if (true == m_isLoop)
		m_iTEState |= CTexture_Effect::STATE_LOOP;
	else
		m_iTEState &= ~CTexture_Effect::STATE_LOOP;

	if (true == m_isDistortion)
		m_iTEState |= CTexture_Effect::STATE_DISTORTION;
	else
		m_iTEState &= ~CTexture_Effect::STATE_DISTORTION;

	if (true == m_isBlur)
		m_iTEState |= CTexture_Effect::STATE_BLUR;
	else
		m_iTEState &= ~CTexture_Effect::STATE_BLUR;

}

void CController_EffectTool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
