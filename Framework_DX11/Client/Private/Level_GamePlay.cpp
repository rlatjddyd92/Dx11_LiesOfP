#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"
#include <io.h>
#include<fstream>

#include "FreeCamera.h"
#include "GameInstance.h"
#include "GameInterface_Controller.h"

#include "Effect_Container.h"
#include "Effect_Manager.h"
#include "StaticObj.h"
#include "Player.h"

CLevel_Tool::CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Tool::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Effect()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Monster()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Paticle()))
		return E_FAIL;	
	
	if (FAILED(Read_Map_Data()))
		return E_FAIL;	
	

	// 2024-11-10 김성용
	// 게임 인터페이스를 플레이 모드로 설정 
	// 게임 플레이에 필요한 인벤, 플레이 화면, 스탯 화면 등을 상황에 따라 보여 주도록 설정 
	GET_GAMEINTERFACE->SetPlayMode(true);

	CEffect_Container::EFFECT_DESC desc = {};

	desc.fRotationPerSec = XMConvertToRadians(90.f);
	desc.fSpeedPerSec = 1.f;
	desc.iLevelIndex = LEVEL_GAMEPLAY;
	desc.pParentMatrix = nullptr;
	desc.pSocketMatrix = nullptr;
	desc.vPos = { 0.f, 0.f, 0.f };
	desc.vRotation = { 0.f, 0.f, 0.f };
	desc.vScale = { 1.f, 1.f, 1.f };

	CEffect_Manager::Get_Instance()->Add_Effect_ToLayer(LEVEL_GAMEPLAY, TEXT("TEST_Effect"), &desc);

	return S_OK;
}

void CLevel_Tool::Update(_float fTimeDelta)
{

}

HRESULT CLevel_Tool::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));
	return S_OK;
}

HRESULT CLevel_Tool::Ready_Lights()
{
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	LIGHT_DESC			LightDesc{};

	ZeroMemory(&LightDesc, sizeof LightDesc);
	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	//ZeroMemory(&LightDesc, sizeof LightDesc);
	//LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(10.f, 3.f, 10.f, 1.f);
	//LightDesc.fRange = 7.f;	
	//LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	//LightDesc.vAmbient = /*_float4(0.4f, 0.2f, 0.2f, 1.f);*/_float4(0.f, 0.f, 0.f, 0.f);
	//LightDesc.vSpecular = LightDesc.vDiffuse;

	//if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;

	//ZeroMemory(&LightDesc, sizeof LightDesc);
	//LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(20.f, 3.f, 10.f, 1.f);
	//LightDesc.fRange = 7.f;
	//LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
	//LightDesc.vAmbient = /*_float4(0.2f, 0.4f, 0.2f, 1.f);*/_float4(0.f, 0.f, 0.f, 0.f);
	//LightDesc.vSpecular = LightDesc.vDiffuse;

	//if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Camera()
{
	CFreeCamera::CAMERA_FREE_DESC		Desc{};

	Desc.fSensor = 0.2f;
	Desc.vEye = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 1.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), &Desc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_Tool::Ready_Layer_BackGround()
{

//	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_NavDataObj"))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_ForkLift"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Effect()
{

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Monster()
{
	/*for (size_t i = 0; i < 10; i++)
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster"))))
			return E_FAIL;
	}*/
	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_CarcassBigA"))))
	//	return E_FAIL;

	// 24-11-15 김성용
	// 직교 UI 테스트용 코드 
	// 테스트 후에 for문만 제거하기 

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_CarcassBigA"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_SimonManusP1"))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Paticle()
{
	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Expolosion"))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Particle"), TEXT("Prototype_GameObject_Particle_Snow"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tool::Ready_Layer_Player()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player")));

	return S_OK;
}

HRESULT CLevel_Tool::Read_Map_Data()
{
	const char cFile[128] = "../Bin/DataFiles/Map_Data.dat";
	ifstream fin(cFile, ios::in | ios::binary);

	//	fin.open("../Bin/Map_Data.txt");
	if (!fin.is_open())    // 파일 열었다면
	{
		MSG_BOX(TEXT("파일 읽기를 실패했어요.."));
		return E_FAIL;
	}

	string line;
	_uint LayerCount = { 0 };
	fin.read(reinterpret_cast<char*>(&LayerCount), sizeof(LayerCount));	//레이어 개수

	string strLayerTag = {};
	std::string::size_type iStrSize = { 0 };
	_uint iObjectCount = 0;
	int i = 0;
	int iObjectType = 0;
	int iObjectListIndex = 0;
	_float3 fPos = {};
	_float3 fScaled = {};
	_float3 fRot = {};
	_int iCellNum = {};
	_uint iCellRoomNum = {};

	while (i < (_int)LayerCount)
	{
		fin.read(reinterpret_cast<char*>(&iStrSize), sizeof(iStrSize));
		strLayerTag.resize(iStrSize);
		fin.read(&strLayerTag[0], iStrSize);
		fin.read(reinterpret_cast<char*>(&iObjectCount), sizeof(iObjectCount));

		for (_int j = 0; j < (_int)iObjectCount; ++j)
		{
			if (strLayerTag != "Layer_Light")	//조명 제외 모든 오브젝트 불러오기
			{
				OBJECT_DEFAULT_DESC pDesc = {};
				fin.read(reinterpret_cast<char*>(&pDesc), sizeof(pDesc));

				if (strLayerTag == "Layer_Map")
				{
					CStaticObj::STATICOBJ_DESC staticObjDesc = {};
					int bufferSize = WideCharToMultiByte(CP_ACP, 0, pDesc.szModelTag, -1, NULL, 0, NULL, NULL);
					WideCharToMultiByte(CP_ACP, 0, pDesc.szModelTag, -1, staticObjDesc.szModelTag, bufferSize, NULL, NULL);

					staticObjDesc.vPosition = pDesc.vPosition;
					staticObjDesc.vScale = pDesc.vScale;
					staticObjDesc.vRotation = pDesc.vRotation;
					staticObjDesc.isInstance = pDesc.isInstance;
					staticObjDesc.iRenderGroupID = pDesc.iID;
					staticObjDesc.bShadow = pDesc.bShadow;

					if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Map"), TEXT("Prototype_GameObject_StaticObj"), &staticObjDesc)))
						return E_FAIL;
				} 

				else if (strLayerTag == "Layer_InteractObject")
				{
					 if (wcscmp(pDesc.szModelTag, TEXT("SK_DLV_Stargazer_01")) == 0)
					 {
						 if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Stargazer"), TEXT("Prototype_GameObject_Stargazer"), &pDesc)))
							 return E_FAIL;
					 }
				}

			}
			else
			{
				LIGHT_DESC pDesc = {};

				fin.read(reinterpret_cast<char*>(&pDesc), sizeof(pDesc));
				if (FAILED(m_pGameInstance->Add_Light(pDesc)))
					return E_FAIL;
			}
		}
		++i;
	}
	fin.close();
	return S_OK;
}

CLevel_Tool * CLevel_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Tool*		pInstance = new CLevel_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tool::Free()
{
	__super::Free();
	// 인스턴싱을 할 모델들을 모아둔 매니저 클리어하기
	m_pGameInstance->Clear_Instance();
}
