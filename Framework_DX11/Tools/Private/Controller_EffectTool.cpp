#include "stdafx.h"
#include "ImGui_Manager.h"
#include "Controller_EffectTool.h"
#include "GameInstance.h"

#include "Layer.h"

#include "Particle_Effect.h"
#include "Texture_Effect.h"
#include "Mesh_Effect.h"
#include "Trail_Effect_OP.h"

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

void CController_EffectTool::Render()
{
	bool bDemo = true;
	ImGui::ShowDemoWindow(&bDemo);
	if (ImGui::CollapsingHeader("Texture"))
	{
		RenderGroup_Selection();
	}

	if (ImGui::CollapsingHeader("Particle Effect"))
	{
		Particle_Check();
	}

	if (ImGui::CollapsingHeader("Texture Effect"))
	{
		TE_Check();
	}

	if (ImGui::CollapsingHeader("Mesh Effect"))
	{
		Mesh_Check();
	}

	if (ImGui::CollapsingHeader("Trail Effect"))
	{
		//Trail_OP_Check();
	}

	ImGui::Begin("EffectList");
	if (ImGui::Button("Load Effect"))
	{
		ImGuiFileDialog::Instance()->OpenDialog(
			"LoadFile",					// vKey
			"Select a File",			// vTitle
			"Effects{.PE, .TE, .ME}",   // vFilters
			IGFD::FileDialogConfig()	// vConfig (기본 설정)
		);
	}
	Load_Effect();

#pragma region PE
	ImGui::InputText("Particle Name", m_szParticleName, IM_ARRAYSIZE(m_szParticleName));

	Select_Particle();
	if (ImGui::Button("Create Particle"))
	{
		Add_Particle();
	}
	if (ImGui::Button("Update Particle"))
	{
		Update_Particle();
	}
	if (ImGui::Button("Get Particle"))
	{
		Get_Particle();
	}
	if (ImGui::Button("Delete Particle"))
	{
		Delete_Particle();
	}
	if (ImGui::Button("Clear Particle"))
	{
		Clear_Particle();
	}
#pragma endregion

#pragma region TE
	ImGui::InputText("Texture Name", m_szTextureName, IM_ARRAYSIZE(m_szTextureName));

	Select_TE();
	if (ImGui::Button("Create Texture Effect"))
	{
		Add_TE();
	}
	if (ImGui::Button("Update Texture Effect"))
	{
		Update_TE();
	}
	if (ImGui::Button("Get Texture Effect"))
	{
		Get_TE();
	}
	if (ImGui::Button("Delete Texture Effect"))
	{
		Delete_TE();
	}
	if (ImGui::Button("Clear Texture Effect"))
	{
		Clear_TE();
	}
#pragma endregion

#pragma region ME
	ImGui::InputText("Mesh Name", m_szMeshName, IM_ARRAYSIZE(m_szMeshName));

	Select_Mesh();
	if (ImGui::Button("Create Mesh Effect"))
	{
		Add_Mesh();
	}
	if (ImGui::Button("Update Mesh Effect"))
	{
		Update_Mesh();
	}
	if (ImGui::Button("Get Mesh Effect"))
	{
		Get_Mesh();
	}
	if (ImGui::Button("Delete Mesh Effect"))
	{
		Delete_Mesh();
	}
	if (ImGui::Button("Clear Mesh Effect"))
	{
		Clear_ME();
	}
#pragma endregion
	ImGui::End();

	ImGui::Begin("Effect_Container");
	ImGui::SeparatorText("Effect Container");
	ImGui::InputText("Effect Container Name", m_szEffectContainerName, IM_ARRAYSIZE(m_szEffectContainerName));
	Check_EffectContainer();
	if (ImGui::Button("Add Effect Conainer"))
	{
		Add_EffectContainer();
	}
	if (ImGui::Button("Back Effect Conainer"))
	{
		Back_EffectContainer();
	}
	if (ImGui::Button("Reset Effect Conainer"))
	{
		Reset_EffectContainer();
	}
	if (ImGui::Button("Delete Effect Container"))
	{
		Delete_EffectContainer();
	}

	if (ImGui::Button("Save Effect Conainer"))
	{
		ImGuiFileDialog::Instance()->OpenDialog(
			"SaveFile",           // vKey
			"Select a File",           // vTitle
			".bin",                  // vFilters
			IGFD::FileDialogConfig()   // vConfig (기본 설정)
		);
	}
	Save_EffectContainer();

	if (ImGui::Button("Load Effect Conainer"))
	{
		ImGuiFileDialog::Instance()->OpenDialog(
			"LoadContainer",           // vKey
			"Select a File",           // vTitle
			"Effects{.EC}",                  // vFilters
			IGFD::FileDialogConfig()   // vConfig (기본 설정)
		);
	}
	Load_EffectContainer();

	ImGui::End();

	ImGui::EndTabItem();
}

void CController_EffectTool::RenderGroup_Selection()
{
	Texture_Selection();

	ImGui::SeparatorText("Render Group");
	ImGui::RadioButton("RG_Nonblend", &m_RenderDesc.iRenderGroup, CRenderer::RG_NONBLEND);
	ImGui::SameLine();
	ImGui::RadioButton("RG_Effect", &m_RenderDesc.iRenderGroup, CRenderer::RG_EFFECT);
	ImGui::SameLine();
	ImGui::RadioButton("RG_Blend", &m_RenderDesc.iRenderGroup, CRenderer::RG_BLEND);

	switch (m_RenderDesc.iRenderGroup)
	{
	case CRenderer::RG_NONBLEND:
		m_RenderDesc.iPpState = 0;		
		break;

	case CRenderer::RG_EFFECT:
		ImGui::SeparatorText("Post Processing");
		ImGui::Checkbox("PP_None", &m_EffectPP[PP_NONE]);
		ImGui::SameLine();
		ImGui::Checkbox("PP_Blur", &m_EffectPP[PP_BLUR]);
		ImGui::SameLine();
		ImGui::Checkbox("PP_Distortion", &m_EffectPP[PP_DISTORTION]);
		
		if (true == m_EffectPP[PP_NONE])
			m_RenderDesc.iPpState |= CEffect_Base::PP_NONE;
		else
			m_RenderDesc.iPpState &= ~CEffect_Base::PP_NONE;

		if (true == m_EffectPP[PP_BLUR])
			m_RenderDesc.iPpState |= CEffect_Base::PP_BLUR;
		else
			m_RenderDesc.iPpState &= ~CEffect_Base::PP_BLUR;

		if (true == m_EffectPP[PP_DISTORTION])
			m_RenderDesc.iPpState |= CEffect_Base::PP_DISTORTION;
		else
			m_RenderDesc.iPpState &= ~CEffect_Base::PP_DISTORTION;
		break;

	case CRenderer::RG_BLEND:
		m_RenderDesc.iPpState = 0;
		break;
	}

}

HRESULT CController_EffectTool::Add_Particle()
{
	m_ParticleDesc.fRotationPerSec = XMConvertToRadians(90.f);
	m_ParticleDesc.fSpeedPerSec = 1.f;
	m_ParticleDesc.iLevelIndex = LEVEL_TOOL;
	m_ParticleDesc.pParentMatrix = { nullptr };
	m_ParticleDesc.RenderDesc = m_RenderDesc;

	size_t iCharLen = strlen(m_szParticleName);
	size_t iConvertChars = 0;
	mbstowcs_s(&iConvertChars, m_ParticleDesc.szEffectName, sizeof(m_ParticleDesc.szEffectName) / sizeof(_tchar), m_szParticleName, iCharLen);

	wcsncpy_s(m_ParticleDesc.TextDesc.szDiffuseTexturTag, m_Texture_PrototypeTags[m_iSelected_DiffuseTextureIndex].c_str(),
		sizeof(m_ParticleDesc.TextDesc.szDiffuseTexturTag) / sizeof(_tchar));

	wcsncpy_s(m_ParticleDesc.TextDesc.szMaskTextureTag_1, m_Texture_PrototypeTags[m_iSelected_MaskTextureIndex_1].c_str(),
		sizeof(m_ParticleDesc.TextDesc.szMaskTextureTag_1) / sizeof(_tchar));

	wcsncpy_s(m_ParticleDesc.TextDesc.szMaskTextureTag_2, m_Texture_PrototypeTags[m_iSelected_MaskTextureIndex_2].c_str(),
		sizeof(m_ParticleDesc.TextDesc.szMaskTextureTag_1) / sizeof(_tchar));

	wcsncpy_s(m_ParticleDesc.TextDesc.szNormalTextureTag, m_Texture_PrototypeTags[m_iSelected_NormalTextureIndex].c_str(),
		sizeof(m_ParticleDesc.TextDesc.szNormalTextureTag) / sizeof(_tchar));

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Effect"), &m_ParticleDesc)))
		return E_FAIL;

	return S_OK;
}

void CController_EffectTool::Particle_Check()
{
	if (ImGui::TreeNode("Particle Initialize"))
	{
		ImGui::SeparatorText("Initialize");

		ImGui::InputInt	("Num Instance", (_int*)&m_ParticleDesc.BufferDesc.iNumInstance);
		ImGui::InputFloat3("Center Instance", (_float*)&m_ParticleDesc.BufferDesc.vCenter);
		ImGui::InputFloat3("Range Instance", (_float*)&m_ParticleDesc.BufferDesc.vRange);
		ImGui::InputFloat3("ExceptRange Instance", (_float*)&m_ParticleDesc.BufferDesc.vExceptRange);
		ImGui::InputFloat2("Size Instance", (_float*)&m_ParticleDesc.BufferDesc.vSize);
		ImGui::InputFloat2("Speed Instance", (_float*)&m_ParticleDesc.BufferDesc.vSpeed);
		ImGui::InputFloat2("LifeTime Instance", (_float*)&m_ParticleDesc.BufferDesc.vLifeTime);
		ImGui::InputFloat4("MinColor Instance", (_float*)&m_ParticleDesc.BufferDesc.vMinColor);
		ImGui::InputFloat4("MaxColor Instance", (_float*)&m_ParticleDesc.BufferDesc.vMaxColor);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Particle Type/State"))
	{
		ImGui::SeparatorText("Normal");
		ImGui::RadioButton("Spread", (_int*)&m_ParticleDesc.DefaultDesc.eType, CParticle_Effect::TYPE_SPREAD);
		ImGui::SameLine();
		ImGui::RadioButton("Move", (_int*)&m_ParticleDesc.DefaultDesc.eType, CParticle_Effect::TYPE_MOVE);
		ImGui::SameLine();
		ImGui::RadioButton("Converge", (_int*)&m_ParticleDesc.DefaultDesc.eType, CParticle_Effect::TYPE_CONVERGE);

		ImGui::SeparatorText("Compute State");
		ImGui::Checkbox("Orbit", &m_ParticleStates[BOOL_ORBIT]);
		ImGui::SameLine();
		ImGui::Checkbox("Random", &m_ParticleStates[BOOL_RANDOM]);
		ImGui::SameLine();
		ImGui::Checkbox("Loop", &m_ParticleStates[BOOL_LOOP]);

		ImGui::Checkbox("Accel", &m_ParticleStates[BOOL_ACCEL]);
		ImGui::SameLine();
		ImGui::Checkbox("Decel", &m_ParticleStates[BOOL_DECEL]);

		ImGui::SeparatorText("Geom State");
		ImGui::Checkbox("Grow", &m_ParticleStates[BOOL_GROW]);
		ImGui::SameLine();
		ImGui::Checkbox("Shrink", &m_ParticleStates[BOOL_SHRINK]);
		ImGui::SameLine();
		ImGui::Checkbox("Rotation", &m_ParticleStates[BOOL_ROTATION]);

		ImGui::SeparatorText("Shader");
		ImGui::InputInt("Shader Index", (_int*)&m_ParticleDesc.DefaultDesc.iShaderIndex);

		Set_ParticleState();
		
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Particle Additional"))
	{
		ImGui::SeparatorText("Transform");
		ImGui::InputFloat3("Pos", (_float*)&m_ParticleDesc.DefaultDesc.vPos);
		ImGui::InputFloat3("Rotation", (_float*)&m_ParticleDesc.DefaultDesc.vRotation);
		ImGui::InputFloat3("Scale", (_float*)&m_ParticleDesc.DefaultDesc.vScale);

		ImGui::SeparatorText("Particle Action"); 
		ImGui::InputFloat4("Pivot", (_float*)&m_ParticleDesc.DefaultDesc.vPivot);
		ImGui::InputFloat("Gravity", (_float*)&m_ParticleDesc.DefaultDesc.fGravity);
		ImGui::InputFloat4("Move Dir", (_float*)&m_ParticleDesc.DefaultDesc.vMoveDir);

		ImGui::InputFloat2("Tex Devide", (_float*)&m_ParticleDesc.DefaultDesc.vTexDevide);
		ImGui::InputFloat("Sprite Speed", &m_ParticleDesc.DefaultDesc.fSpriteSpeed);

		ImGui::InputFloat2("Scaling", (_float*)&m_ParticleDesc.DefaultDesc.vScaling);

		ImGui::InputFloat("Start Rotation", &m_ParticleDesc.DefaultDesc.fStartRotation);
		ImGui::InputFloat("Rotation Per Second", (_float*)&m_ParticleDesc.DefaultDesc.fRotationPerSecond);

		ImGui::SeparatorText("Particle Orbit");
		ImGui::InputFloat3("Orbit Axis", (_float*)&m_ParticleDesc.DefaultDesc.vOrbitAxis);
		ImGui::InputFloat("Orbit Angle", (_float*)&m_ParticleDesc.DefaultDesc.fOrbitAngle);

		ImGui::SeparatorText("Particle Random");
		ImGui::InputFloat("Random Time Interval", (_float*)&m_ParticleDesc.DefaultDesc.fRandomTimeInterval);
		ImGui::InputFloat("Random Move Ratio", (_float*)&m_ParticleDesc.DefaultDesc.fRandomMoveRatio);

		ImGui::SeparatorText("Particle Accel");
		ImGui::InputFloat("Accel Speed", (_float*)&m_ParticleDesc.DefaultDesc.fAccelSpeed);
		ImGui::InputFloat("Accel Limit", (_float*)&m_ParticleDesc.DefaultDesc.fAccelLimit);

		ImGui::TreePop();
	}
}

void CController_EffectTool::Update_Particle()
{
	CParticle_Effect* pParticle = static_cast<CParticle_Effect*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_Particle"), m_iParticleIndex));

	if (nullptr == pParticle)
		return;

	pParticle->Set_Desc(m_ParticleDesc);

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

	ImGui::ListBox("Particle List", &m_iParticleIndex, Items.data(), (_int)Items.size(), 5);
}

void CController_EffectTool::Get_Particle()
{
	CParticle_Effect* pParticle = static_cast<CParticle_Effect*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_Particle"), m_iParticleIndex));

	if (nullptr == pParticle)
		return;

	m_ParticleDesc = pParticle->Get_Desc();
	m_RenderDesc = m_ParticleDesc.RenderDesc;

	Set_PpState();

#pragma region BOOL
	if (CVIBuffer_Point_Instance::STATE_ORBIT == (m_ParticleDesc.DefaultDesc.iComputeState & CVIBuffer_Point_Instance::STATE_ORBIT))
		m_ParticleStates[BOOL_ORBIT] = true;
	else
		m_ParticleStates[BOOL_ORBIT] = false;

	if (CVIBuffer_Point_Instance::STATE_RANDOM == (m_ParticleDesc.DefaultDesc.iComputeState & CVIBuffer_Point_Instance::STATE_RANDOM))
		m_ParticleStates[BOOL_RANDOM] = true;
	else
		m_ParticleStates[BOOL_RANDOM] = false;
	
	if (CVIBuffer_Point_Instance::STATE_LOOP == (m_ParticleDesc.DefaultDesc.iComputeState & CVIBuffer_Point_Instance::STATE_LOOP))
		m_ParticleStates[BOOL_LOOP] = true;
	else
		m_ParticleStates[BOOL_LOOP] = false;

	if (CVIBuffer_Point_Instance::STATE_ACCEL == (m_ParticleDesc.DefaultDesc.iComputeState & CVIBuffer_Point_Instance::STATE_ACCEL))
		m_ParticleStates[BOOL_ACCEL] = true;
	else
		m_ParticleStates[BOOL_ACCEL] = false;

	if (CVIBuffer_Point_Instance::STATE_DECEL == (m_ParticleDesc.DefaultDesc.iComputeState & CVIBuffer_Point_Instance::STATE_DECEL))
		m_ParticleStates[BOOL_DECEL] = true;
	else
		m_ParticleStates[BOOL_DECEL] = false;

	if (CParticle_Effect::PS_GROW == (m_ParticleDesc.DefaultDesc.iGeomState & CParticle_Effect::PS_GROW))
		m_ParticleStates[BOOL_GROW] = true;
	else
		m_ParticleStates[BOOL_GROW] = false;

	if (CParticle_Effect::PS_SHRINK == (m_ParticleDesc.DefaultDesc.iGeomState & CParticle_Effect::PS_SHRINK))
		m_ParticleStates[BOOL_SHRINK] = true;
	else
		m_ParticleStates[BOOL_SHRINK] = false;

	if (CParticle_Effect::PS_ROTATION == (m_ParticleDesc.DefaultDesc.iGeomState & CParticle_Effect::PS_ROTATION))
		m_ParticleStates[BOOL_ROTATION] = true;
	else
		m_ParticleStates[BOOL_ROTATION] = false;
#pragma endregion
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
		if (iIndex == m_iParticleIndex)
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

void CController_EffectTool::Clear_Particle()
{
	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_Particle"));

	if (nullptr == pLayer)
		return;

	auto& ObjectList = pLayer->Get_ObjectList();

	for (auto& Object : ObjectList)
	{
		Safe_Release(Object);
	}
	ObjectList.clear();
}

HRESULT CController_EffectTool::Add_TE()
{
	m_TextureDesc.fRotationPerSec	= XMConvertToRadians(90.f);
	m_TextureDesc.fSpeedPerSec		= 1.f;
	m_TextureDesc.iLevelIndex		= LEVEL_TOOL;
	m_TextureDesc.RenderDesc		= m_RenderDesc;
	
	m_TextureDesc.pParentMatrix = { nullptr };
	size_t iCharLen = strlen(m_szTextureName);
	size_t iConvertChars = 0;
	mbstowcs_s(&iConvertChars, m_TextureDesc.szEffectName, sizeof(m_TextureDesc.szEffectName) / sizeof(_tchar), m_szTextureName, iCharLen);

	wcsncpy_s(m_TextureDesc.TextDesc.szDiffuseTexturTag, m_Texture_PrototypeTags[m_iSelected_DiffuseTextureIndex].c_str(),
		sizeof(m_TextureDesc.TextDesc.szDiffuseTexturTag) / sizeof(_tchar));

	wcsncpy_s(m_TextureDesc.TextDesc.szMaskTextureTag_1, m_Texture_PrototypeTags[m_iSelected_MaskTextureIndex_1].c_str(),
		sizeof(m_TextureDesc.TextDesc.szMaskTextureTag_1) / sizeof(_tchar));

	wcsncpy_s(m_TextureDesc.TextDesc.szMaskTextureTag_2, m_Texture_PrototypeTags[m_iSelected_MaskTextureIndex_2].c_str(),
		sizeof(m_TextureDesc.TextDesc.szMaskTextureTag_1) / sizeof(_tchar));

	wcsncpy_s(m_TextureDesc.TextDesc.szNormalTextureTag, m_Texture_PrototypeTags[m_iSelected_NormalTextureIndex].c_str(),
		sizeof(m_TextureDesc.TextDesc.szNormalTextureTag) / sizeof(_tchar));


	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_TextureEffect"), TEXT("Prototype_GameObject_Texture_Effect"), &m_TextureDesc)))
		return E_FAIL;

	return S_OK;
}

void CController_EffectTool::TE_Check()
{
	if (ImGui::TreeNode("TE Initialize"))
	{
		ImGui::SeparatorText("Initialize");

		ImGui::InputFloat("Duration TE", (_float*)&m_TextureDesc.DefaultDesc.fDuration);
		ImGui::InputFloat4("Color TE", (_float*)&m_TextureDesc.DefaultDesc.vColor);
		ImGui::InputFloat2("Divide TE", (_float*)&m_TextureDesc.DefaultDesc.vDivide);
		ImGui::InputFloat("Sprite Speed TE", (_float*)&m_TextureDesc.DefaultDesc.fSpriteSpeed);

		ImGui::SeparatorText("Transform");
		ImGui::InputFloat3("Pos TE", (_float*)&m_TextureDesc.DefaultDesc.vPos);
		ImGui::InputFloat("Rotation Angle TE", (_float*)&m_TextureDesc.DefaultDesc.fRotationAngle);
		ImGui::InputFloat("Rotation Speed TE", (_float*)&m_TextureDesc.DefaultDesc.fRotationSpeed);
		ImGui::InputFloat3("Start Scale TE", (_float*)&m_TextureDesc.DefaultDesc.vStartScale);
		ImGui::InputFloat3("Scaling Speed TE", (_float*)&m_TextureDesc.DefaultDesc.vScalingSpeed);

		ImGui::SeparatorText("Blending");
		ImGui::InputFloat("Start Alpha TE", (_float*)&m_TextureDesc.DefaultDesc.fAlpha);
		ImGui::InputFloat("Alpha Speed TE", (_float*)&m_TextureDesc.DefaultDesc.fAlphaSpeed);
		ImGui::SeparatorText("Shader");
		ImGui::InputInt("Shader Index", (_int*)&m_TextureDesc.DefaultDesc.iShaderIndex);

		ImGui::TreePop();
	}
}

void CController_EffectTool::Update_TE()
{
	CTexture_Effect* pTE = static_cast<CTexture_Effect*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_TextureEffect"), m_iTextureIndex));

	if (nullptr == pTE)
		return;

	pTE->Set_Desc(m_TextureDesc);
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

	ImGui::ListBox("TE List", &m_iTextureIndex, Items.data(), (_int)Items.size(), 5);

}

void CController_EffectTool::Get_TE()
{
	CTexture_Effect* pTextureEffect = static_cast<CTexture_Effect*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_TextureEffect"), m_iTextureIndex));

	if (nullptr == pTextureEffect)
		return;

	m_TextureDesc = pTextureEffect->Get_Desc();
	m_RenderDesc = m_TextureDesc.RenderDesc;

	Set_PpState();
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
		if (iIndex == m_iTextureIndex)
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

void CController_EffectTool::Clear_TE()
{
	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_TextureEffect"));

	if (nullptr == pLayer)
		return;

	auto& ObjectList = pLayer->Get_ObjectList();

	for (auto& Object : ObjectList)
	{
		Safe_Release(Object);
	}
	ObjectList.clear();
}

HRESULT CController_EffectTool::Add_Mesh()
{
	m_MeshDesc.fRotationPerSec = XMConvertToRadians(90.f);
	m_MeshDesc.fSpeedPerSec = 1.f;
	m_MeshDesc.iLevelIndex = LEVEL_TOOL;
	m_MeshDesc.pParentMatrix = { nullptr };
	m_MeshDesc.RenderDesc = m_RenderDesc;

	size_t iCharLen = strlen(m_szMeshName);
	size_t iConvertChars = 0;
	mbstowcs_s(&iConvertChars, m_MeshDesc.szEffectName, sizeof(m_MeshDesc.szEffectName) / sizeof(_tchar), m_szMeshName, iCharLen);


	wcsncpy_s(m_MeshDesc.TextDesc.szModelTag, m_Model_PrototypeTags[m_iSelected_ModelIndex].c_str(),
		sizeof(m_MeshDesc.TextDesc.szModelTag) / sizeof(_tchar));

	wcsncpy_s(m_MeshDesc.TextDesc.szDiffuseTexturTag, m_Texture_PrototypeTags[m_iSelected_DiffuseTextureIndex].c_str(),
		sizeof(m_MeshDesc.TextDesc.szDiffuseTexturTag) / sizeof(_tchar));

	wcsncpy_s(m_MeshDesc.TextDesc.szMaskTextureTag_1, m_Texture_PrototypeTags[m_iSelected_MaskTextureIndex_1].c_str(),
		sizeof(m_MeshDesc.TextDesc.szMaskTextureTag_1) / sizeof(_tchar));

	wcsncpy_s(m_MeshDesc.TextDesc.szMaskTextureTag_2, m_Texture_PrototypeTags[m_iSelected_MaskTextureIndex_2].c_str(),
		sizeof(m_MeshDesc.TextDesc.szMaskTextureTag_2) / sizeof(_tchar));
	
	wcsncpy_s(m_MeshDesc.TextDesc.szNormalTextureTag, m_Texture_PrototypeTags[m_iSelected_NormalTextureIndex].c_str(),
		sizeof(m_MeshDesc.TextDesc.szNormalTextureTag) / sizeof(_tchar));

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_MeshEffect"), TEXT("Prototype_GameObject_Mesh_Effect"), &m_MeshDesc)))
		return E_FAIL;

	return S_OK;
}


void CController_EffectTool::Mesh_Check()
{
	if (ImGui::TreeNode("Model"))
	{
		vector<string> strArray;

		for (const auto& strPrototype : m_Model_PrototypeTags)
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

		ImGui::ListBox("Model List", &m_iSelected_ModelIndex, szItems.data(), (_int)szItems.size(), 5);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("ME Initialize"))
	{
		ImGui::SeparatorText("Initialize");

		_wstring strNormalText = m_Model_PrototypeTags[m_iSelected_ModelIndex];
		int iSize = WideCharToMultiByte(CP_UTF8, 0, &strNormalText[0], (int)strNormalText.size(), nullptr, 0, nullptr, nullptr);
		string strNormalTo(iSize, 0);
		WideCharToMultiByte(CP_UTF8, 0, &strNormalText[0], (int)strNormalText.size(), &strNormalTo[0], iSize, nullptr, nullptr);
		_char* szNormal = const_cast<_char*>(strNormalTo.data());
		ImGui::InputText("Model", szNormal, IM_ARRAYSIZE(szNormal));
		
		ImGui::SeparatorText("Transform");																	
		ImGui::InputFloat("Duration ME", (_float*)&m_MeshDesc.DefaultDesc.fDuration);
		ImGui::InputFloat4("Color ME", (_float*)&m_MeshDesc.DefaultDesc.vColor);

		ImGui::SeparatorText("Tile Move");
		ImGui::InputFloat2("Tile Repeat", (_float*)&m_MeshDesc.DefaultDesc.vTileRepeat);
		ImGui::InputFloat2("Tile Move Dir", (_float*)&m_MeshDesc.DefaultDesc.vTileMoveDir);
		ImGui::InputFloat("Tile Move Speed", (_float*)&m_MeshDesc.DefaultDesc.fTileMoveSpeed);

		ImGui::SeparatorText("Tile Transform");
		ImGui::InputFloat3("Pos ME", (_float*)&m_MeshDesc.DefaultDesc.vPos);
		ImGui::InputFloat3("Start Rotation ME", (_float*)&m_MeshDesc.DefaultDesc.vStartRotation);
		ImGui::InputFloat4("Rotation Axis ME", (_float*)&m_MeshDesc.DefaultDesc.vRotationAxis);
		ImGui::InputFloat("Rotation Speed ME", (_float*)&m_MeshDesc.DefaultDesc.fRotationSpeed);
		ImGui::InputFloat3("Start Scale ME", (_float*)&m_MeshDesc.DefaultDesc.vStartScale);
		ImGui::InputFloat3("Scaling Speed ME", (_float*)&m_MeshDesc.DefaultDesc.vScalingSpeed);

		ImGui::SeparatorText("Blending");
		ImGui::InputFloat("Start Alpha ME", (_float*)&m_MeshDesc.DefaultDesc.fAlpha);
		ImGui::InputFloat("Alpha Speed ME", (_float*)&m_MeshDesc.DefaultDesc.fAlphaSpeed);
		
		ImGui::SeparatorText("Shader");
		ImGui::InputInt("Shader Index", (_int*)&m_MeshDesc.DefaultDesc.iShaderIndex);

		ImGui::TreePop();
	}
}

void CController_EffectTool::Update_Mesh()
{
	CMesh_Effect* pME = static_cast<CMesh_Effect*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_MeshEffect"), m_iMeshIndex));

	if (nullptr == pME)
		return;

	pME->Set_Desc(m_MeshDesc);

}

void CController_EffectTool::Select_Mesh()
{
	vector<string> IndexLavels;
	vector<const _char*> Items;

	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_MeshEffect"));

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

	ImGui::ListBox("ME List", &m_iMeshIndex, Items.data(), (_int)Items.size(), 5);
}

void CController_EffectTool::Get_Mesh()
{
	CMesh_Effect* pMeshEffect = static_cast<CMesh_Effect*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_MeshEffect"), m_iMeshIndex));

	if (nullptr == pMeshEffect)
		return;

	m_MeshDesc = pMeshEffect->Get_Desc();
	m_RenderDesc = m_MeshDesc.RenderDesc;
	Set_PpState();
}

void CController_EffectTool::Delete_Mesh()
{
	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_MeshEffect"));

	if (nullptr == pLayer)
		return;

	auto& ObjectList = pLayer->Get_ObjectList();
	_uint iIndex = { 0 };
	for (auto& iter = ObjectList.begin(); iter != ObjectList.end();)
	{
		if (iIndex == m_iMeshIndex)
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

void CController_EffectTool::Clear_ME()
{
	CLayer* pLayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_MeshEffect"));

	if (nullptr == pLayer)
		return;

	auto& ObjectList = pLayer->Get_ObjectList();

	for (auto& Object : ObjectList)
	{
		Safe_Release(Object);
	}
	ObjectList.clear();
}

HRESULT CController_EffectTool::Check_EffectContainer()
{
	CGameObject* pGameObject = m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_Effect"), 0);

	if (nullptr == pGameObject)
		return E_FAIL;

	CEffect_Container* pContainer = static_cast<CEffect_Container*>(pGameObject);
	_wstring strNameTag = pContainer->Get_ContainerName();
	int iSize = WideCharToMultiByte(CP_UTF8, 0, strNameTag.c_str(), (int)strNameTag.size(), nullptr, 0, nullptr, nullptr);
	string strTo(iSize, 0);
	WideCharToMultiByte(CP_UTF8, 0, strNameTag.c_str(), (int)strNameTag.size(), &strTo[0], iSize, nullptr, nullptr);

	ImGui::Text(strTo.c_str());
	ImGui::Text("Is Existing!!!");

	return S_OK;
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
			pParticleDescs.emplace_back(static_cast<CParticle_Effect*>(elem)->Get_InitDesc_Ptr());
		}
	}

	CLayer* pTELayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_TextureEffect"));
	if (nullptr != pTELayer)
	{
		for (auto& elem : pTELayer->Get_ObjectList())
		{
			pTextureDesc.emplace_back(static_cast<CTexture_Effect*>(elem)->Get_InitDesc_Ptr());
		}
	}

	CLayer* pMELayer = m_pGameInstance->Find_Layer(LEVEL_TOOL, TEXT("Layer_MeshEffect"));
	if (nullptr != pMELayer)
	{
		for (auto& elem : pMELayer->Get_ObjectList())
		{
			pMeshDesc.emplace_back(static_cast<CMesh_Effect*>(elem)->Get_InitDesc_Ptr());
		}
	}

	CEffect_Container::EFFECT_DESC EffectDesc = {};
	EffectDesc.fRotationPerSec = XMConvertToRadians(90.f);
	EffectDesc.fSpeedPerSec = 1.f;
	EffectDesc.iLevelIndex = LEVEL_TOOL;
	EffectDesc.pParticleEffect_Descs = pParticleDescs;
	EffectDesc.pTextureEffect_Descs = pTextureDesc;
	EffectDesc.pMeshEffect_Descs = pMeshDesc;
	size_t iCharLen = strlen(m_szEffectContainerName);
	size_t iConvertChars = 0;
	mbstowcs_s(&iConvertChars, EffectDesc.szEffectContainerName, sizeof(EffectDesc.szEffectContainerName) / sizeof(_tchar), m_szEffectContainerName, iCharLen);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Continaer"), &EffectDesc)))
		return E_FAIL;

	Clear_Particle();
	Clear_ME();
	Clear_TE();

	return S_OK;
}

HRESULT CController_EffectTool::Back_EffectContainer()
{
	CEffect_Container* pEffectContainer = static_cast<CEffect_Container*>(m_pGameInstance->Find_Object(LEVEL_TOOL, TEXT("Layer_Effect"), 0));

	if (nullptr == pEffectContainer)
		return E_FAIL;

	if(FAILED(pEffectContainer->Add_Effects_To_Layer()))
		return E_FAIL;
	Delete_EffectContainer();
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

				infile.read(reinterpret_cast<_char*>(TestDesc.szEffectName), sizeof(TestDesc.szEffectName));	// 이거 키로 쓰고
				infile.read(reinterpret_cast<_char*>(&TestDesc.RenderDesc), sizeof(TestDesc.RenderDesc));		// 이거 버퍼 초기화에 쓰고
				infile.read(reinterpret_cast<_char*>(&TestDesc.DefaultDesc), sizeof(TestDesc.DefaultDesc));		// 이거 버퍼 초기화에 쓰고
				infile.read(reinterpret_cast<_char*>(&TestDesc.TextDesc), sizeof(TestDesc.TextDesc));			// 이게 실제로 적용되는 거.
				infile.read(reinterpret_cast<_char*>(&TestDesc.BufferDesc), sizeof(TestDesc.BufferDesc));			// 이게 실제로 적용되는 거.
				
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

				TestDesc.pParentMatrix = { nullptr };
				TestDesc.fRotationPerSec = XMConvertToRadians(90.f);
				TestDesc.fSpeedPerSec = 1.f;
				TestDesc.iLevelIndex = LEVEL_TOOL;

				infile.read(reinterpret_cast<_char*>(TestDesc.szEffectName), sizeof(TestDesc.szEffectName));	// 이거 키로 쓰고
				infile.read(reinterpret_cast<_char*>(&TestDesc.RenderDesc), sizeof(TestDesc.RenderDesc));		// 이거 버퍼 초기화에 쓰고
				infile.read(reinterpret_cast<_char*>(&TestDesc.DefaultDesc), sizeof(TestDesc.DefaultDesc));		// 이거 텍스처 초기화에 쓰고
				infile.read(reinterpret_cast<_char*>(&TestDesc.TextDesc), sizeof(TestDesc.TextDesc));		// 이거는 그대로 가져다 쓰고

				infile.close();

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_TextureEffect"), TEXT("Prototype_GameObject_Texture_Effect"), &TestDesc)))
					return E_FAIL;
			}
			else if (TEXT("ME") == strExtention)
			{
				CMesh_Effect::MESH_EFFECT_DESC TestDesc = {};

				ifstream infile(wFilePath, ios::binary);

				if (!infile.is_open())
					return E_FAIL;

				TestDesc.pParentMatrix = { nullptr };
				TestDesc.fRotationPerSec = XMConvertToRadians(90.f);
				TestDesc.fSpeedPerSec = 1.f;
				TestDesc.iLevelIndex = LEVEL_TOOL;

				infile.read(reinterpret_cast<_char*>(TestDesc.szEffectName), sizeof(TestDesc.szEffectName));	// 이거 키로 쓰고
				infile.read(reinterpret_cast<_char*>(&TestDesc.RenderDesc), sizeof(TestDesc.RenderDesc));		// 이거 버퍼 초기화에 쓰고
				infile.read(reinterpret_cast<_char*>(&TestDesc.DefaultDesc), sizeof(TestDesc.DefaultDesc));		// 이거 텍스처 초기화에 쓰고
				infile.read(reinterpret_cast<_char*>(&TestDesc.TextDesc), sizeof(TestDesc.TextDesc));		// 이거는 그대로 가져다 쓰고

				infile.close();

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_MeshEffect"), TEXT("Prototype_GameObject_Texture_Effect"), &TestDesc)))
					return E_FAIL;
			}

		}

		// 대화 상자 닫기
		ImGuiFileDialog::Instance()->Close();
	}

	return S_OK;
}

HRESULT CController_EffectTool::Load_EffectContainer()
{
	if (ImGuiFileDialog::Instance()->Display("LoadContainer"))
	{
		// 사용자가 파일을 선택한 경우
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			CEffect_Container::EFFECT_DESC desc = {};
			desc.fRotationPerSec = XMConvertToRadians(90.f);
			desc.fSpeedPerSec = 1.f;
			desc.iLevelIndex = LEVEL_TOOL;

			string FileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			strcpy_s(m_szEffectContainerName, FileName.c_str());
			
			size_t iCharLen = strlen(m_szEffectContainerName);
			size_t iConvertChars = 0;
			mbstowcs_s(&iConvertChars, desc.szEffectContainerName, sizeof(desc.szEffectContainerName) / sizeof(_tchar), m_szEffectContainerName, iCharLen);

			CEffect_Container* pEffectContainer = static_cast<CEffect_Container*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Effect_Continaer"), &desc));
			if (nullptr == pEffectContainer)
				return E_FAIL;


			// 선택된 파일 경로 가져오기
			string filePath = ImGuiFileDialog::Instance()->GetFilePathName();

			int size_needed = MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, nullptr, 0);
			_wstring wFilePath(size_needed, 0);
			MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, &wFilePath[0], size_needed);
	
			pEffectContainer->Load_EffectContainer(wFilePath);

			m_pGameInstance->Add_Object_ToLayer(LEVEL_TOOL, TEXT("Layer_Effect"), pEffectContainer);
		}

		// 대화 상자 닫기
		ImGuiFileDialog::Instance()->Close();
	}

	return S_OK;
}

void CController_EffectTool::Set_ParticleState()
{
	if (true == m_ParticleStates[BOOL_ORBIT])
		m_ParticleDesc.DefaultDesc.iComputeState |= CVIBuffer_Point_Instance::STATE_ORBIT;
	else
		m_ParticleDesc.DefaultDesc.iComputeState &= ~CVIBuffer_Point_Instance::STATE_ORBIT;

	if (true == m_ParticleStates[BOOL_RANDOM])
		m_ParticleDesc.DefaultDesc.iComputeState |= CVIBuffer_Point_Instance::STATE_RANDOM;
	else
		m_ParticleDesc.DefaultDesc.iComputeState &= ~CVIBuffer_Point_Instance::STATE_RANDOM;

	if (true == m_ParticleStates[BOOL_LOOP])
		m_ParticleDesc.DefaultDesc.iComputeState |= CVIBuffer_Point_Instance::STATE_LOOP;
	else
		m_ParticleDesc.DefaultDesc.iComputeState &= ~CVIBuffer_Point_Instance::STATE_LOOP;

	if (true == m_ParticleStates[BOOL_ACCEL])
		m_ParticleDesc.DefaultDesc.iComputeState |= CVIBuffer_Point_Instance::STATE_ACCEL;
	else
		m_ParticleDesc.DefaultDesc.iComputeState &= ~CVIBuffer_Point_Instance::STATE_ACCEL;

	if (true == m_ParticleStates[BOOL_DECEL])
		m_ParticleDesc.DefaultDesc.iComputeState |= CVIBuffer_Point_Instance::STATE_DECEL;
	else
		m_ParticleDesc.DefaultDesc.iComputeState &= ~CVIBuffer_Point_Instance::STATE_DECEL;

	if (true == m_ParticleStates[BOOL_GROW])
		m_ParticleDesc.DefaultDesc.iGeomState |= CParticle_Effect::PS_GROW;
	else
		m_ParticleDesc.DefaultDesc.iGeomState &= ~CParticle_Effect::PS_GROW;
	if (true == m_ParticleStates[BOOL_SHRINK])
		m_ParticleDesc.DefaultDesc.iGeomState |= CParticle_Effect::PS_SHRINK;
	else
		m_ParticleDesc.DefaultDesc.iGeomState &= ~CParticle_Effect::PS_SHRINK;
	if (true == m_ParticleStates[BOOL_ROTATION])
		m_ParticleDesc.DefaultDesc.iGeomState |= CParticle_Effect::PS_ROTATION;
	else
		m_ParticleDesc.DefaultDesc.iGeomState &= ~CParticle_Effect::PS_ROTATION;
}


void CController_EffectTool::Texture_Selection()
{
	ImGui::SeparatorText("Texture Select");

	ImGui::RadioButton("Diffuse", &m_iTextureSelect, CEffect_Base::TEXTURE_DIFFUSE);
	ImGui::SameLine();
	ImGui::RadioButton("Mask_1", &m_iTextureSelect, CEffect_Base::TEXTURE_MASK_1);
	ImGui::SameLine();
	ImGui::RadioButton("Mask_2", &m_iTextureSelect, CEffect_Base::TEXTURE_MASK_2);
	ImGui::SameLine();
	ImGui::RadioButton("Normal", &m_iTextureSelect, CEffect_Base::TEXTURE_NORMAL);

	vector<string> strTextureArray;
	for (const auto& strPrototype : m_Texture_PrototypeTags)
	{
		int iSize = WideCharToMultiByte(CP_UTF8, 0, &strPrototype[0], (int)strPrototype.size(), nullptr, 0, nullptr, nullptr);
		string strTo(iSize, 0);
		WideCharToMultiByte(CP_UTF8, 0, &strPrototype[0], (int)strPrototype.size(), &strTo[0], iSize, nullptr, nullptr);
		strTextureArray.emplace_back(strTo);
	}

	vector<const _char*> szTextureArray;
	for (const auto& str : strTextureArray)
	{
		szTextureArray.emplace_back(str.c_str());
	}

	switch (m_iTextureSelect)
	{
	case 0:
		ImGui::ListBox("Diffuse Texture List", &m_iSelected_DiffuseTextureIndex, szTextureArray.data(), (_int)szTextureArray.size(), 5);
		break;
	case 1:
		ImGui::ListBox("Mask Texture List_1", &m_iSelected_MaskTextureIndex_1, szTextureArray.data(), (_int)szTextureArray.size(), 5);
		break;
	case 2:
		ImGui::ListBox("Mask Texture List_2", &m_iSelected_MaskTextureIndex_2, szTextureArray.data(), (_int)szTextureArray.size(), 5);
		break;
	case 3:
		ImGui::ListBox("Normal Texture List", &m_iSelected_NormalTextureIndex, szTextureArray.data(), (_int)szTextureArray.size(), 5);
		break;
	}
	
	_wstring strDiffuseText = m_Texture_PrototypeTags[m_iSelected_DiffuseTextureIndex];
	int iSize = WideCharToMultiByte(CP_UTF8, 0, &strDiffuseText[0], (int)strDiffuseText.size(), nullptr, 0, nullptr, nullptr);
	string strDiffuseTo(iSize, 0);
	WideCharToMultiByte(CP_UTF8, 0, &strDiffuseText[0], (int)strDiffuseText.size(), &strDiffuseTo[0], iSize, nullptr, nullptr);
	_char* szDiffuse = const_cast<_char*>(strDiffuseTo.data());
	ImGui::InputText("Diffuse Texture", szDiffuse, IM_ARRAYSIZE(szDiffuse));

	_wstring strMaskText_1 = m_Texture_PrototypeTags[m_iSelected_MaskTextureIndex_1];
	iSize = WideCharToMultiByte(CP_UTF8, 0, &strMaskText_1[0], (int)strMaskText_1.size(), nullptr, 0, nullptr, nullptr);
	string strMaskTo_1(iSize, 0);
	WideCharToMultiByte(CP_UTF8, 0, &strMaskText_1[0], (int)strMaskText_1.size(), &strMaskTo_1[0], iSize, nullptr, nullptr);
	_char* szMask_1 = const_cast<_char*>(strMaskTo_1.data());
	ImGui::InputText("Mask_1 Texture", szMask_1, IM_ARRAYSIZE(szMask_1));

	_wstring strMaskText_2 = m_Texture_PrototypeTags[m_iSelected_MaskTextureIndex_2];
	iSize = WideCharToMultiByte(CP_UTF8, 0, &strMaskText_2[0], (int)strMaskText_2.size(), nullptr, 0, nullptr, nullptr);
	string strMaskTo_2(iSize, 0);
	WideCharToMultiByte(CP_UTF8, 0, &strMaskText_2[0], (int)strMaskText_2.size(), &strMaskTo_2[0], iSize, nullptr, nullptr);
	_char* szMask_2 = const_cast<_char*>(strMaskTo_2.data());
	ImGui::InputText("Mask_2 Texture", szMask_2, IM_ARRAYSIZE(szMask_2));

	_wstring strNormalText = m_Texture_PrototypeTags[m_iSelected_NormalTextureIndex];
	iSize = WideCharToMultiByte(CP_UTF8, 0, &strNormalText[0], (int)strNormalText.size(), nullptr, 0, nullptr, nullptr);
	string strNormalTo(iSize, 0);
	WideCharToMultiByte(CP_UTF8, 0, &strNormalText[0], (int)strNormalText.size(), &strNormalTo[0], iSize, nullptr, nullptr);
	_char* szNormal = const_cast<_char*>(strNormalTo.data());
	ImGui::InputText("Normal Texture", szNormal, IM_ARRAYSIZE(szNormal));

}

void CController_EffectTool::Set_PpState()
{
	if (PP_NONE & m_RenderDesc.iPpState)
		m_EffectPP[PP_NONE] = true;
	else
		m_EffectPP[PP_NONE] = false;

	if (PP_BLUR & m_RenderDesc.iPpState)
		m_EffectPP[PP_BLUR] = true;
	else
		m_EffectPP[PP_BLUR] = false;

	if (PP_DISTORTION & m_RenderDesc.iPpState)
		m_EffectPP[PP_DISTORTION] = true;
	else
		m_EffectPP[PP_DISTORTION] = false;
}




void CController_EffectTool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
