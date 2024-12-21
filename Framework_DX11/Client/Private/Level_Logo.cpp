#include "stdafx.h"
#include "..\Public\Level_Logo.h"
#include "Level_Loading.h"
#include "GameInstance.h"

#include "FreeCamera.h"

#include "GameInterface_Controller.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	// 24-11-19 김성용
	// 게임 인터페이스 인게임 모드 해제
	GET_GAMEINTERFACE->SetIngame(false);


	m_pGameInstance->Play_BGM(TEXT("MU_MS_Title_Piano_03.wav"), &g_fBGMVolume);

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	GET_GAMEINTERFACE->OpenMainPage(); // 메인 페이지 진입 

	if ((GetKeyState(VK_RETURN) & 0x8000) /*|| (GET_GAMEINTERFACE->Get_Start_Loading())*/)
	{
		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;

		GET_GAMEINTERFACE->Open_Loading_Page(); // 로딩 진행 
	}
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Lights()
{
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(0.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Camera()
{
	CFreeCamera::CAMERA_FREE_DESC		Desc{};

	Desc.fSensor = 0.2f;
	Desc.vEye = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 1.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 500.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;

	CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), &Desc));
	if (nullptr == pCamera)
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, strLayerTag,
		TEXT("Prototype_GameObject_BackGround"))))
		return E_FAIL;

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Logo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	m_pGameInstance->Clear_Lights();

	__super::Free();
}
