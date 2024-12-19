#include "stdafx.h"

#include "Level_Tool.h"

#include "FreeCamera.h"
#include "GameInstance.h"

#include "ImGui_Manager.h"

#include "NonAnimModel.h"

#include "Trail_Effect_TP.h"
#include "Trail_Effect_MP.h"
#include "Trail_Effect_OP.h"

#include "Mesh_Effect.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
  	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Effect()))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	m_pImGui_Manager = CImGui_Manager::Get_Instance();
	if (FAILED(m_pImGui_Manager->Initialize(m_pDevice, m_pContext)))
		return E_FAIL;

	CNonAnimModel::NONMODEL_DESC Desc{};
	Desc.vPosition = { 0.f, 0.f, 0.f };
	Desc.vScale = { 1.5f,1.5f,1.5f };
	Desc.vRotation = { 0.f, 0.f, 0.f };
	Desc.iRenderGroupID = 0;

	//strcpy_s(Desc.szModelTag, "Prototype_Component_Model_Raxasia_Sword_CutScene");
	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Torch"), TEXT("Prototype_GameObject_NonAnim"), &Desc)))
	//	return E_FAIL;
	/*strcpy_s(Desc.szModelTag, "Prototype_AnimModel_Raxasia_Sword01");
	strcpy_s(Desc.szModelTag, "Prototype_Component_Model_Player_Arm");
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Torch"), TEXT("Prototype_GameObject_NonAnim"), &Desc)))
		return E_FAIL;*/

	//strcpy_s(Desc.szModelTag, "Prototype_Component_Model_FlameSword");
	strcpy_s(Desc.szModelTag, "Prototype_Component_Model_Player_Arm");
	//strcpy_s(Desc.szModelTag, "Prototype_Component_Model_Raxasia_Sword_CutScene");
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Offset"), TEXT("Prototype_GameObject_NonAnim"), &Desc)))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_CutSceneodHand"))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_ScissorHandle"))))
	//	return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("툴 프로그램입니다."));

	// ImGui_Manager는 Render_ImGui만 호출하면 동작함
	m_pImGui_Manager->Render_ImGui();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(-1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.6f, 0.5f, 0.4f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	//ZeroMemory(&LightDesc, sizeof LightDesc);
	//LightDesc.eType = LIGHT_DESC::TYPE_SPOT;
	//LightDesc.vPosition = _float4(0.f, 10.f, 0.f, 1.f);
	//LightDesc.fRange = 40.f;
	//LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	//LightDesc.vAmbient = /*_float4(0.2f, 0.4f, 0.2f, 1.f);*/_float4(0.f, 0.f, 0.f, 0.f);
	//LightDesc.vSpecular = LightDesc.vDiffuse;

	//if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera()
{
	CCamera::CAMERA_DESC		Desc{};

	Desc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 300.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	/*if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObjects()
{
	
	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

	Safe_Release(m_pImGui_Manager);

	// 인스턴싱을 할 모델들을 모아둔 매니저 클리어하기
	m_pGameInstance->Clear_Instance();
}
