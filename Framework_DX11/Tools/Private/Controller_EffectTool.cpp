#include "stdafx.h"
#include "ImGui_Manager.h"
#include "Controller_EffectTool.h"
#include "GameInstance.h"

#include "Layer.h"
#include "Particle_Effect.h"
#include "Texture_Effect.h"
#include "Effect_Container.h"

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

void CController_EffectTool::TextureCheck()
{
	if (ImGui::TreeNode("Diffuse/Mask Texture"))
	{
		vector<string> strArray;

		for (const auto& strPrototype : m_Diffuse_PrototypeTags)
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

		
		ImGui::RadioButton("Diffuse", &m_iTextureSelect, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Mask_1", &m_iTextureSelect, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Mask_2", &m_iTextureSelect, 2);
		
		switch (m_iTextureSelect)
		{
		case 0:
			ImGui::ListBox("Diffuse/Mask Texture List", &m_iSelected_DiffuseTextureIndex, szItems.data(), (_int)szItems.size(), 5);
			break;
		case 1:
			ImGui::ListBox("Diffuse/Mask Texture List", &m_iSelected_MaskTextureIndex_1, szItems.data(), (_int)szItems.size(), 5);
			break;
		case 2: 
			ImGui::ListBox("Diffuse/Mask Texture List", &m_iSelected_MaskTextureIndex_2, szItems.data(), (_int)szItems.size(), 5);
			break;
		}
		
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Normal Texture"))
	{
		vector<string> strArray;

		for (const auto& strPrototype : m_Normal_PrototypeTags)
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

		ImGui::ListBox("Normal Texture List", &m_iSelected_NormalTextureIndex, szItems.data(), (_int)szItems.size(), 5);


		ImGui::TreePop();
	}
}

HRESULT CController_EffectTool::Add_Particle()
{
	CParticle_Effect::PARTICLE_EFFECT_DESC desc = {};
	desc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.fSpeedPerSec = 1.f;
	desc.iLevelIndex = LEVEL_TOOL;

	desc.pParentMatrix = { nullptr };

	size_t iCharLen = strlen(m_szEffectName);
	size_t iConvertChars = 0;
	mbstowcs_s(&iConvertChars, desc.strEffectName, sizeof(desc.strEffectName) / sizeof(_tchar), m_szEffectName, iCharLen);

	desc.BufferDesc.iNumInstance = m_iNumInstance;
	desc.BufferDesc.vCenter = m_vCenter;
	desc.BufferDesc.vExceptRange = m_vExceptRange;
	desc.BufferDesc.vLifeTime = m_vLifeTime;
	desc.BufferDesc.vMaxColor = m_vMaxColor;
	desc.BufferDesc.vMinColor = m_vMinColor;
	desc.BufferDesc.vRange = m_vRange;
	desc.BufferDesc.vSize = m_vSize;
	desc.BufferDesc.vSpeed = m_vSpeed;

	CParticle_Effect::DEFAULT_DESC DefaultDesc = {};
	DefaultDesc.eType = (CParticle_Effect::TYPE)m_iParticleType;
	DefaultDesc.iState = m_iInstanceState;
	DefaultDesc.fRenderRatio = m_fRandomRatio;
	DefaultDesc.vPivot = m_vPivot;
	DefaultDesc.fGravity = m_fGravity;
	DefaultDesc.vMoveDir = m_vMoveDir;
	DefaultDesc.vTexDevide = m_vTexDevide;
	DefaultDesc.vScaling = m_vScaling;
	DefaultDesc.fAngle = m_fParticleAngle;
	DefaultDesc.iShaderIndex = m_iParticleShaderIndex;
	DefaultDesc.iRenderState = m_iParticleRenderState;
	DefaultDesc.iParticleState = m_iParticleState;
	DefaultDesc.fSpriteSpeed = m_fParticle_SpriteSpeed;
	DefaultDesc.fStartRotation = m_fStartRotation;

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

	desc.InitDesc.DefaultDesc = DefaultDesc;
	desc.InitDesc.OrbitDesc = OrbitDesc;
	desc.InitDesc.RandomDesc = RandomDesc;
	desc.InitDesc.AccelDesc = AccelDesc;
	desc.InitDesc.TransformDesc = TransformDesc;

	wcsncpy_s(desc.InitDesc.TextureDesc.strDiffuseTexturTag, m_Diffuse_PrototypeTags[m_iSelected_DiffuseTextureIndex].c_str(), sizeof(desc.InitDesc.TextureDesc.strDiffuseTexturTag) / sizeof(_tchar));

	wcsncpy_s(desc.InitDesc.TextureDesc.strNomralTextureTag, m_Normal_PrototypeTags[m_iSelected_NormalTextureIndex].c_str(), sizeof(desc.InitDesc.TextureDesc.strNomralTextureTag) / sizeof(_tchar));
	
	wcsncpy_s(desc.InitDesc.TextureDesc.strMaskTextureTag, m_Diffuse_PrototypeTags[m_iSelected_MaskTextureIndex_1].c_str(), sizeof(desc.InitDesc.TextureDesc.strMaskTextureTag) / sizeof(_tchar));

	if (m_iMax_ParticleShaderIndex < m_iParticleShaderIndex)
		m_iParticleShaderIndex = m_iMax_ParticleShaderIndex;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Effect"), &desc)))
		return E_FAIL;

	return S_OK;
}

void CController_EffectTool::Particle_Check()
{

	if (ImGui::TreeNode("Particle Initialize"))
	{
		ImGui::SeparatorText("Initialize");

		_wstring strDiffuseText = m_Diffuse_PrototypeTags[m_iSelected_DiffuseTextureIndex];
		int iSize = WideCharToMultiByte(CP_UTF8, 0, &strDiffuseText[0], (int)strDiffuseText.size(), nullptr, 0, nullptr, nullptr);
		string strDiffuseTo(iSize, 0);
		WideCharToMultiByte(CP_UTF8, 0, &strDiffuseText[0], (int)strDiffuseText.size(), &strDiffuseTo[0], iSize, nullptr, nullptr);
		_char* szDiffuse = const_cast<_char*>(strDiffuseTo.data());
		ImGui::InputText("Diffuse", szDiffuse, IM_ARRAYSIZE(szDiffuse));

		_wstring strNormalText = m_Normal_PrototypeTags[m_iSelected_NormalTextureIndex];
		iSize = WideCharToMultiByte(CP_UTF8, 0, &strNormalText[0], (int)strNormalText.size(), nullptr, 0, nullptr, nullptr);
		string strNormalTo(iSize, 0);
		WideCharToMultiByte(CP_UTF8, 0, &strNormalText[0], (int)strNormalText.size(), &strNormalTo[0], iSize, nullptr, nullptr);
		_char* szNormal = const_cast<_char*>(strNormalTo.data());
		ImGui::InputText("Normal", szNormal, IM_ARRAYSIZE(szNormal));

		_wstring strMaskText_1 = m_Diffuse_PrototypeTags[m_iSelected_MaskTextureIndex_1];
		iSize = WideCharToMultiByte(CP_UTF8, 0, &strMaskText_1[0], (int)strMaskText_1.size(), nullptr, 0, nullptr, nullptr);
		string strMaskTo_1(iSize, 0);
		WideCharToMultiByte(CP_UTF8, 0, &strMaskText_1[0], (int)strMaskText_1.size(), &strMaskTo_1[0], iSize, nullptr, nullptr);
		_char* szMask_1 = const_cast<_char*>(strMaskTo_1.data());
		ImGui::InputText("Mask_1", szMask_1, IM_ARRAYSIZE(szMask_1));

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

		ImGui::Checkbox("Grow", &m_bGrow);
		ImGui::SameLine();
		ImGui::Checkbox("Shrink", &m_bShrink);
		ImGui::SameLine();
		ImGui::Checkbox("Rotation", &m_bRotation);

		ImGui::Checkbox("NonBlend", &m_bNonBlend);
		ImGui::SameLine();
		ImGui::Checkbox("NonLight", &m_bNonLight);
		ImGui::SameLine();
		ImGui::Checkbox("Blur", &m_bBlur);


		ImGui::SeparatorText("Shader");
		ImGui::InputInt("Shader Index", (_int*)&m_iParticleShaderIndex);



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

		ImGui::InputFloat2("Tex Devide", (_float*)&m_vTexDevide);
		ImGui::InputFloat("Sprite Speed", &m_fParticle_SpriteSpeed);

		ImGui::InputFloat2("Scaling", (_float*)&m_vScaling);

		ImGui::InputFloat("Rotation Angle", &m_fStartRotation);
		ImGui::InputFloat("Rotation Per Second", (_float*)&m_fParticleAngle);

		ImGui::SeparatorText("Particle Orbit");
		ImGui::InputFloat3("Orbit Axis", (_float*)&m_vOrbitAxis);
		ImGui::InputFloat("Orbit Angle", (_float*)&m_fAngle);

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
	DefaultDesc.iState = m_iInstanceState;
	DefaultDesc.fRenderRatio = m_fRenderRatio;
	DefaultDesc.vPivot = m_vPivot;
	DefaultDesc.fGravity = m_fGravity;
	DefaultDesc.vMoveDir = m_vMoveDir;
	DefaultDesc.vTexDevide = m_vTexDevide;
	DefaultDesc.vScaling = m_vScaling;
	DefaultDesc.fAngle = m_fParticleAngle;
	DefaultDesc.iShaderIndex = m_iParticleShaderIndex;
	DefaultDesc.iRenderState = m_iParticleRenderState;
	DefaultDesc.iParticleState = m_iParticleState;
	DefaultDesc.fSpriteSpeed = m_fParticle_SpriteSpeed;
	DefaultDesc.fStartRotation = m_fStartRotation;

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

}

void CController_EffectTool::Select_Particle()
{
	vector<string> IndexLavels;
	vector<const _char*> Items;

	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Particle"));
	
	if (nullptr != pLayer)	
	{
		for (const auto& elem : pLayer->Get_ObjectList())
		{
			_wstring strEffectName = static_cast<CEffect_Base*>(elem)->Get_EffectName();

			_int size_needed = WideCharToMultiByte(CP_UTF8, 0, strEffectName.c_str(), -1, nullptr, 0, nullptr, nullptr);
			std::string str(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, strEffectName.c_str(), -1, &str[0], size_needed, nullptr, nullptr);

			IndexLavels.emplace_back(str);
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
	m_iInstanceState = DefaultDesc.iState;
	m_fRenderRatio = DefaultDesc.fRenderRatio;
	m_vPivot = DefaultDesc.vPivot;
	m_fGravity = DefaultDesc.fGravity;
	m_vMoveDir = DefaultDesc.vMoveDir;
	m_vTexDevide = DefaultDesc.vTexDevide;
	m_vScaling = DefaultDesc.vScaling;
	m_fParticleAngle = DefaultDesc.fAngle;
	m_iParticleRenderState = DefaultDesc.iRenderState;
	m_iParticleShaderIndex = DefaultDesc.iShaderIndex;
	m_iParticleState = DefaultDesc.iParticleState;
	m_fParticle_SpriteSpeed = DefaultDesc.fSpriteSpeed;
	m_fStartRotation = DefaultDesc.fStartRotation;

	m_vOrbitAxis = OrbitDesc.vOrbitAxis;
	m_fAngle = OrbitDesc.fAngle;

	m_fTimeInterval = RandomDesc.fTimeInterval;
	m_fRandomRatio = RandomDesc.fRandomRatio;

	m_fAccelLimit = AccelDesc.fAccelLimit;
	m_fAccelSpeed = AccelDesc.fAccelSpeed;


	if (CVIBuffer_Point_Instance::STATE_ORBIT == (m_iInstanceState & CVIBuffer_Point_Instance::STATE_ORBIT))
		m_bOrbit = true;
	else
		m_bOrbit = false;

	if (CVIBuffer_Point_Instance::STATE_RANDOM == (m_iInstanceState & CVIBuffer_Point_Instance::STATE_RANDOM))
		m_bRandom = true;
	else
		m_bRandom = false;

	if (CVIBuffer_Point_Instance::STATE_LOOP == (m_iInstanceState & CVIBuffer_Point_Instance::STATE_LOOP))
		m_bLoop = true;
	else
		m_bLoop = false;

	if (CVIBuffer_Point_Instance::STATE_ACCEL == (m_iInstanceState & CVIBuffer_Point_Instance::STATE_ACCEL))
		m_bAccel = true;
	else
		m_bAccel = false;

	if (CVIBuffer_Point_Instance::STATE_DECEL == (m_iInstanceState & CVIBuffer_Point_Instance::STATE_DECEL))
		m_bDecel = true;
	else
		m_bDecel = false;

	if (CParticle_Effect::RS_BLUR == (m_iParticleRenderState & CParticle_Effect::RS_BLUR))
		m_bBlur = true;
	else
		m_bBlur = false;

	if (CParticle_Effect::RS_NONBLEND == (m_iParticleRenderState & CParticle_Effect::RS_NONBLEND))
		m_bNonBlend = true;
	else
		m_bNonBlend = false;

	if (CParticle_Effect::RS_NONLIGHT == (m_iParticleRenderState & CParticle_Effect::RS_NONLIGHT))
		m_bNonLight = true;
	else
		m_bNonLight = false;

	if (CParticle_Effect::PS_GROW == (m_iParticleState & CParticle_Effect::PS_GROW))
		m_bGrow = true;
	else
		m_bGrow = false;

	if (CParticle_Effect::PS_SHRINK == (m_iParticleState & CParticle_Effect::PS_SHRINK))
		m_bShrink = true;
	else
		m_bShrink = false;

	if (CParticle_Effect::PS_ROTATION == (m_iParticleState & CParticle_Effect::PS_ROTATION))
		m_bRotation = true;
	else
		m_bRotation = false;
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
	
	desc.pParentMatrix = { nullptr };
	size_t iCharLen = strlen(m_szEffectName);
	size_t iConvertChars = 0;
	mbstowcs_s(&iConvertChars, desc.strEffectName, sizeof(desc.strEffectName) / sizeof(_tchar), m_szEffectName, iCharLen);

	wcsncpy_s(desc.TextureDesc.strDiffuseTexturTag, m_Diffuse_PrototypeTags[m_iSelected_DiffuseTextureIndex].c_str(), sizeof(desc.TextureDesc.strDiffuseTexturTag) / sizeof(_tchar));
	wcsncpy_s(desc.TextureDesc.strNomralTextureTag, m_Normal_PrototypeTags[m_iSelected_NormalTextureIndex].c_str(), sizeof(desc.TextureDesc.strNomralTextureTag) / sizeof(_tchar));
	wcsncpy_s(desc.TextureDesc.strMaskTextureTag_1, m_Diffuse_PrototypeTags[m_iSelected_MaskTextureIndex_1].c_str(), sizeof(desc.TextureDesc.strMaskTextureTag_1) / sizeof(_tchar));
	wcsncpy_s(desc.TextureDesc.strMaskTextureTag_2, m_Diffuse_PrototypeTags[m_iSelected_MaskTextureIndex_2].c_str(), sizeof(desc.TextureDesc.strMaskTextureTag_2) / sizeof(_tchar));

	desc.ActionDesc.iState				= m_iTEState;
	desc.ActionDesc.iShaderIndex		= m_iTEShaderIndex;
	desc.ActionDesc.fDuration			= m_fDuration;
	desc.ActionDesc.vColor				= m_vColor;
	desc.ActionDesc.vDivide				= m_vDivide;
	desc.ActionDesc.fSpriteSpeed		= m_fSpriteSpeed;
	desc.ActionDesc.vPos				= m_vTEPos;
	desc.ActionDesc.fRotationAngle		= m_fRotationAngle;
	desc.ActionDesc.fRotationSpeed		= m_fRotationSpeed;
	desc.ActionDesc.vScale				= m_vTEScale;
	desc.ActionDesc.vScalingSpeed		= m_vScalingSpeed;
	desc.ActionDesc.fAlpha				= m_fAlpha;
	desc.ActionDesc.fAlphaSpeed		= m_fAlphaSpeed;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_TextureEffect"), TEXT("Prototype_GameObject_Texture_Effect"), &desc)))
		return E_FAIL;

	return S_OK;
}

void CController_EffectTool::TE_Check()
{
	if (ImGui::TreeNode("TE Initialize"))
	{
		ImGui::SeparatorText("Initialize");

		_wstring strDiffuseText = m_Diffuse_PrototypeTags[m_iSelected_DiffuseTextureIndex];
		int iSize = WideCharToMultiByte(CP_UTF8, 0, &strDiffuseText[0], (int)strDiffuseText.size(), nullptr, 0, nullptr, nullptr);
		string strDiffuseTo(iSize, 0);
		WideCharToMultiByte(CP_UTF8, 0, &strDiffuseText[0], (int)strDiffuseText.size(), &strDiffuseTo[0], iSize, nullptr, nullptr);
		_char* szDiffuse = const_cast<_char*>(strDiffuseTo.data());
		ImGui::InputText("Diffuse", szDiffuse, IM_ARRAYSIZE(szDiffuse));

		_wstring strNormalText = m_Normal_PrototypeTags[m_iSelected_NormalTextureIndex];
		iSize = WideCharToMultiByte(CP_UTF8, 0, &strNormalText[0], (int)strNormalText.size(), nullptr, 0, nullptr, nullptr);
		string strNormalTo(iSize, 0);
		WideCharToMultiByte(CP_UTF8, 0, &strNormalText[0], (int)strNormalText.size(), &strNormalTo[0], iSize, nullptr, nullptr);
		_char* szNormal = const_cast<_char*>(strNormalTo.data());
		ImGui::InputText("Normal", szNormal, IM_ARRAYSIZE(szNormal));

		_wstring strMaskText_1 = m_Diffuse_PrototypeTags[m_iSelected_MaskTextureIndex_1];
		iSize = WideCharToMultiByte(CP_UTF8, 0, &strMaskText_1[0], (int)strMaskText_1.size(), nullptr, 0, nullptr, nullptr);
		string strMaskTo_1(iSize, 0);
		WideCharToMultiByte(CP_UTF8, 0, &strMaskText_1[0], (int)strMaskText_1.size(), &strMaskTo_1[0], iSize, nullptr, nullptr);
		_char* szMask_1 = const_cast<_char*>(strMaskTo_1.data());
		ImGui::InputText("Mask_1", szMask_1, IM_ARRAYSIZE(szMask_1));

		_wstring strMaskText_2 = m_Diffuse_PrototypeTags[m_iSelected_MaskTextureIndex_2];
		iSize = WideCharToMultiByte(CP_UTF8, 0, &strMaskText_2[0], (int)strMaskText_2.size(), nullptr, 0, nullptr, nullptr);
		string strMaskTo_2(iSize, 0);
		WideCharToMultiByte(CP_UTF8, 0, &strMaskText_2[0], (int)strMaskText_2.size(), &strMaskTo_2[0], iSize, nullptr, nullptr);
		_char* szMask_2 = const_cast<_char*>(strMaskTo_2.data());
		ImGui::InputText("Mask_2", szMask_2, IM_ARRAYSIZE(szMask_2));

		ImGui::InputFloat("Duration TE", (_float*)&m_fDuration);
		ImGui::InputFloat4("Color TE", (_float*)&m_vColor);
		ImGui::InputFloat2("Divide TE", (_float*)&m_vDivide);
		ImGui::InputFloat("Sprite Speed TE", (_float*)&m_fSpriteSpeed);

		ImGui::SeparatorText("Transform");
		ImGui::InputFloat3("Pos TE", (_float*)&m_vTEPos);
		ImGui::InputFloat("Rotation Angle TE", (_float*)&m_fRotationAngle);
		ImGui::InputFloat("Rotation Speed TE", (_float*)&m_fRotationSpeed);
		ImGui::InputFloat3("Scale TE", (_float*)&m_vTEScale);
		ImGui::InputFloat3("Scaling Speed TE", (_float*)&m_vScalingSpeed);

		ImGui::SeparatorText("Blending");
		ImGui::InputFloat("Alpha TE", (_float*)&m_fAlpha);
		ImGui::InputFloat("Alpha Speed TE", (_float*)&m_fAlphaSpeed);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("TE Type/State"))
	{
		ImGui::SeparatorText("State");
		ImGui::Checkbox("Distortion TE", &m_isDistortion);
		ImGui::SameLine();
		ImGui::Checkbox("Blur TE", &m_isBlur);
		ImGui::SameLine();
		ImGui::Checkbox("Blend TE", &m_isBlend);
		ImGui::SameLine();
		ImGui::Checkbox("NonBlend TE", &m_isNonBlend);
		

		ImGui::SeparatorText("Shader");
		ImGui::InputInt("Shader Index", (_int*)&m_iTEShaderIndex);

		Set_TEState();

		ImGui::TreePop();
	}
}

void CController_EffectTool::Update_TE()
{
	CTexture_Effect* pTE = static_cast<CTexture_Effect*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_TextureEffect"), m_iSelectedTEIndex));

	if (nullptr == pTE)
		return;

	CTexture_Effect::ACTION_DESC ActionDesc = {};

	ActionDesc.iState = m_iTEState;
	ActionDesc.iShaderIndex = m_iTEShaderIndex;
	ActionDesc.fDuration = m_fDuration;
	ActionDesc.vColor = m_vColor;
	ActionDesc.vPos = m_vTEPos;
	ActionDesc.fRotationAngle = m_fRotationAngle;
	ActionDesc.fRotationSpeed = m_fRotationSpeed;
	ActionDesc.vScale = m_vTEScale;
	ActionDesc.vScalingSpeed = m_vScalingSpeed;
	ActionDesc.fAlpha = m_fAlpha;
	ActionDesc.fAlphaSpeed = m_fAlphaSpeed;
	ActionDesc.vDivide = m_vDivide;
	ActionDesc.fSpriteSpeed = m_fSpriteSpeed;

	pTE->Set_Desc(ActionDesc);
}

void CController_EffectTool::Select_TE()
{
	vector<string> IndexLavels;
	vector<const _char*> Items;

	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_TextureEffect"));

	if (nullptr != pLayer)
	{
		for (const auto& elem : pLayer->Get_ObjectList())
		{
			_wstring strEffectName = static_cast<CEffect_Base*>(elem)->Get_EffectName();

			_int size_needed = WideCharToMultiByte(CP_UTF8, 0, strEffectName.c_str(), -1, nullptr, 0, nullptr, nullptr);
			std::string str(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, strEffectName.c_str(), -1, &str[0], size_needed, nullptr, nullptr);

			IndexLavels.emplace_back(str);
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

	m_iTEState			= desc.ActionDesc.iState;
	m_iTEShaderIndex	= desc.ActionDesc.iShaderIndex;
	m_fDuration			= desc.ActionDesc.fDuration;
	m_vColor			= desc.ActionDesc.vColor;
	m_fRotationSpeed	= desc.ActionDesc.fRotationSpeed;
	m_vScalingSpeed		= desc.ActionDesc.vScalingSpeed;
	m_fAlpha			= desc.ActionDesc.fAlpha;
	m_fAlphaSpeed		= desc.ActionDesc.fAlphaSpeed;
	m_vDivide			= desc.ActionDesc.vDivide;
	m_fSpriteSpeed		= desc.ActionDesc.fSpriteSpeed;

	//m_vTEPos = desc.vPos;
	//m_vTEScale = desc.vScale;
	//m_fRotationAngle = desc.fRotationAngle;
	

	if (m_iTEState & CTexture_Effect::STATE_DISTORTION)
		m_isDistortion = true;
	else
		m_isDistortion = false;

	if (m_iTEState & CTexture_Effect::STATE_BLUR)
		m_isBlur = true;
	else
		m_isBlur = false;

	if (m_iTEState & CTexture_Effect::STATE_BLEND)
		m_isBlend = true;
	else
		m_isBlend = false;

	if (m_iTEState & CTexture_Effect::STATE_NONBLEND)
		m_isNonBlend = true;
	else
		m_isNonBlend = false;
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

HRESULT CController_EffectTool::Add_EffectContainer()
{
	Delete_EffectContainer();

	vector<void*> pParticleDescs;
	vector<void*> pTextureDesc;
	vector<void*> pMeshDesc;

	CLayer* pPELayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Particle"));
	if (nullptr != pPELayer)
	{
		for (auto& elem : pPELayer->Get_ObjectList())
		{
			pParticleDescs.emplace_back(static_cast<CParticle_Effect*>(elem)->Get_SaveDesc_Ptr());
		}
	}

	CLayer* pTELayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_TextureEffect"));
	if (nullptr != pTELayer)
	{
		for (auto& elem : pTELayer->Get_ObjectList())
		{
			pTextureDesc.emplace_back(static_cast<CTexture_Effect*>(elem)->Get_SaveDesc_Ptr());
		}
	}


	CEffect_Container::EFFECT_DESC EffectDesc = {};
	EffectDesc.fRotationPerSec = XMConvertToRadians(90.f);
	EffectDesc.fSpeedPerSec = 1.f;
	EffectDesc.iLevelIndex = LEVEL_TOOL;
	EffectDesc.pParticleEffect_Descs = pParticleDescs;
	EffectDesc.pTextureEffect_Descs = pTextureDesc;
	EffectDesc.pMeshEffect_Descs = pMeshDesc;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Continaer"), &EffectDesc)))
		return E_FAIL;

	return S_OK;
}

void CController_EffectTool::Reset_EffectContainer()
{
	CGameObject* pGameObject = m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_Effect"), 0);

	if (nullptr == pGameObject)
		return;

	CEffect_Container* pContainer = static_cast<CEffect_Container*>(pGameObject);
	pContainer->Reset_Effects();
}

void CController_EffectTool::Delete_EffectContainer()
{
	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Effect"));
	if (nullptr == pLayer)
		return;
	
	if (0 == pLayer->Get_ObjectList().size())
		return;

	Safe_Release(*(pLayer->Get_ObjectList().begin()));
	pLayer->Get_ObjectList().erase(pLayer->Get_ObjectList().begin());
}

HRESULT CController_EffectTool::Save_EffectContainer()
{
	if (ImGuiFileDialog::Instance()->Display("SaveFile"))
	{
		// 사용자가 파일을 선택한 경우
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			// 선택된 파일 경로 가져오기
			string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

			int size_needed = MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, nullptr, 0);
			std::wstring wFilePath(size_needed, 0);
			MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, &wFilePath[0], size_needed);

			CEffect_Container* pEffectContainer = static_cast<CEffect_Container*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_Effect"), 0));

			if (nullptr == pEffectContainer)
				return E_FAIL;

			if (FAILED(pEffectContainer->Save_Effects(wFilePath)))
				return E_FAIL;
		}

		// 대화 상자 닫기
		ImGuiFileDialog::Instance()->Close();
	}

	return S_OK;
}

HRESULT CController_EffectTool::Load_Effect()
{
	if (ImGuiFileDialog::Instance()->Display("LoadFile"))
	{
		// 사용자가 파일을 선택한 경우
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			// 선택된 파일 경로 가져오기
			string filePath = ImGuiFileDialog::Instance()->GetFilePathName();

			int size_needed = MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, nullptr, 0);
			_wstring wFilePath(size_needed, 0);
			MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, &wFilePath[0], size_needed);


			size_t dotPosition = wFilePath.find_last_of(L'.');

			// 마지막 점 이후부터 끝까지의 문자열을 반환합니다.
			_wstring strExtention = wFilePath.substr(dotPosition + 1);
			strExtention;
			size_t nullPos = strExtention.find(TEXT('\0'));
			if (nullPos != _wstring::npos)
				strExtention.erase(nullPos);
			
			if (TEXT("PE") == strExtention)
			{
				CParticle_Effect::PARTICLE_EFFECT_DESC TestDesc = {};

				ifstream infile(wFilePath, ios::binary);
				
				if (!infile.is_open())
				    return E_FAIL;
				
				TestDesc.pParentMatrix = { nullptr };
				TestDesc.fRotationPerSec = XMConvertToRadians(90.f);
				TestDesc.fSpeedPerSec = 1.f;
				TestDesc.iLevelIndex = LEVEL_TOOL;

				infile.read(reinterpret_cast<_char*>(TestDesc.strEffectName), sizeof(TestDesc.strEffectName));	// 이거 키로 쓰고
				infile.read(reinterpret_cast<_char*>(&TestDesc.BufferDesc), sizeof(TestDesc.BufferDesc));		// 이거 버퍼 초기화에 쓰고
				infile.read(reinterpret_cast<_char*>(&TestDesc.InitDesc), sizeof(TestDesc.InitDesc));			// 이게 실제로 적용되는 거.
				
				infile.close();

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Effect"), &TestDesc)))
					return E_FAIL;

			}
			else if (TEXT("TE") == strExtention)
			{
				CTexture_Effect::TEXTURE_EFFECT_DESC TestDesc = {};

				ifstream infile(wFilePath, ios::binary);

				if (!infile.is_open())
					return E_FAIL;

				infile.read(reinterpret_cast<_char*>(TestDesc.strEffectName), sizeof(TestDesc.strEffectName));	// 이거 키로 쓰고
				infile.read(reinterpret_cast<_char*>(&TestDesc.TextureDesc), sizeof(TestDesc.TextureDesc));		// 이거 텍스처 초기화에 쓰고
				infile.read(reinterpret_cast<_char*>(&TestDesc.ActionDesc), sizeof(TestDesc.ActionDesc));		// 이거는 그대로 가져다 쓰고

				infile.close();

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_TextureEffect"), TEXT("Prototype_GameObject_Texture_Effect"), &TestDesc)))
					return E_FAIL;
			}
			else if (TEXT("ME") == strExtention)
			{
				
			}

		}

		// 대화 상자 닫기
		ImGuiFileDialog::Instance()->Close();
	}


	return S_OK;
}

void CController_EffectTool::Set_EffectName()
{
	ImGui::InputText("Effect Name", m_szEffectName, IM_ARRAYSIZE(m_szEffectName));
}


void CController_EffectTool::Set_ParticleState()
{
	if (true == m_bOrbit)
		m_iInstanceState |= CVIBuffer_Point_Instance::STATE_ORBIT;
	else
		m_iInstanceState &= ~CVIBuffer_Point_Instance::STATE_ORBIT;

	if (true == m_bRandom)
		m_iInstanceState |= CVIBuffer_Point_Instance::STATE_RANDOM;
	else
		m_iInstanceState &= ~CVIBuffer_Point_Instance::STATE_RANDOM;

	if (true == m_bLoop)
		m_iInstanceState |= CVIBuffer_Point_Instance::STATE_LOOP;
	else
		m_iInstanceState &= ~CVIBuffer_Point_Instance::STATE_LOOP;

	if (true == m_bAccel)
		m_iInstanceState |= CVIBuffer_Point_Instance::STATE_ACCEL;
	else
		m_iInstanceState &= ~CVIBuffer_Point_Instance::STATE_ACCEL;

	if (true == m_bDecel)
		m_iInstanceState |= CVIBuffer_Point_Instance::STATE_DECEL;
	else
		m_iInstanceState &= ~CVIBuffer_Point_Instance::STATE_DECEL;

	if (true == m_bBlur)
		m_iParticleRenderState |= CParticle_Effect::RS_BLUR;
	else
		m_iParticleRenderState &= ~CParticle_Effect::RS_BLUR;

	if (true == m_bNonBlend)
		m_iParticleRenderState |= CParticle_Effect::RS_NONBLEND;
	else
		m_iParticleRenderState &= ~CParticle_Effect::RS_NONBLEND;

	if (true == m_bNonLight)
		m_iParticleRenderState |= CParticle_Effect::RS_NONLIGHT;
	else
		m_iParticleRenderState &= ~CParticle_Effect::RS_NONLIGHT;

	if (true == m_bGrow)
		m_iParticleState |= CParticle_Effect::PS_GROW;
	else
		m_iParticleState &= ~CParticle_Effect::PS_GROW;
	if (true == m_bShrink)
		m_iParticleState |= CParticle_Effect::PS_SHRINK;
	else
		m_iParticleState &= ~CParticle_Effect::PS_SHRINK;
	if (true == m_bRotation)
		m_iParticleState |= CParticle_Effect::PS_ROTATION;
	else
		m_iParticleState &= ~CParticle_Effect::PS_ROTATION;
}

void CController_EffectTool::Set_TEState()
{
	if(true == m_isNonBlend)
		m_iTEState |= CTexture_Effect::STATE_NONBLEND;
	else
		m_iTEState &= ~CTexture_Effect::STATE_NONBLEND;

	if (true == m_isBlend)
		m_iTEState |= CTexture_Effect::STATE_BLEND;
	else
		m_iTEState &= ~CTexture_Effect::STATE_BLEND;

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
