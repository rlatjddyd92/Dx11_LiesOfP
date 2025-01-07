#include "stdafx.h"
#include "..\Public\Level_Logo.h"
#include <io.h>
#include<fstream>

#include "Level_Loading.h"
#include "GameInstance.h"

#include "FreeCamera.h"

#include "GameInterface_Controller.h"


#include "StaticObj.h"

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

	if (FAILED(Ready_TitleObject()))
		return E_FAIL;

	if (FAILED(Read_Map_Data()))
		return E_FAIL;

	// 24-11-19 김성용
	// 게임 인터페이스 인게임 모드 해제
	GET_GAMEINTERFACE->SetIngame(false);

	m_pGameInstance->Play_BGM(TEXT("MU_MS_Title_Piano_03.wav"), &g_fBGMVolume);

	DOF_DESC* tDesc = m_pGameInstance->Get_DOFDesc();
	if (nullptr == tDesc)
		return E_FAIL;

	tDesc->isOnDOF = true;
	tDesc->fDOF = 0.003f;

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	GET_GAMEINTERFACE->OpenMainPage(); // 메인 페이지 진입 

	if ((GetKeyState(VK_RETURN) & 0x8000) || (GET_GAMEINTERFACE->Get_Start_Loading()))
	{

		DOF_DESC* tDesc = m_pGameInstance->Get_DOFDesc();
		if (nullptr == tDesc)
			return;

		tDesc->isOnDOF = false;

		if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;

		GET_GAMEINTERFACE->Open_Loading_Page(); // 로딩 진행 

		m_pGameInstance->Play_Effect(TEXT("SE_UI_TitleScreen_Steelheart_Heartbeat_01.wav"), &g_fEffectVolume);
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
	LightDesc.vDiffuse = _float4(0.1f, 0.1f, 0.1f, 1.f);
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
	Desc.vEye = _float4(0.f, 0.5f, 1.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fAspect = (_float)g_iWinSizeX / (_float)g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 300.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Get_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), &Desc));
	if (nullptr == pCamera)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_TitleObject()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_TitleObject"),
		TEXT("Prototype_GameObject_SteelHeart"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, strLayerTag,
	//	TEXT("Prototype_GameObject_BackGround"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Read_Map_Data()
{
	m_pGameInstance->Change_Active_Octree();	//옥트리 끔

	_Vec3	vMinPos = { FLT_MAX ,FLT_MAX ,FLT_MAX };		//World_Octree의 최소 좌표, 최대한 크게 초기화
	_Vec3	vMaxPos = { -FLT_MAX ,-FLT_MAX ,-FLT_MAX };	//World_Octree의 최대 좌표, 최대한 작게 초기화

	const char cFile[128] = "../Bin/DataFiles/Map_Logo_Data.dat";
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

				_int iAreanNum = { 0 };
				fin.read(reinterpret_cast<char*>(&iAreanNum), sizeof(iAreanNum));

				if (strLayerTag == "Layer_Map" || strLayerTag == "Layer_Etc")
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
					staticObjDesc.iCurrentCellNum = pDesc.iCurrentCellNum;
					staticObjDesc.iCurrentArealNum = iAreanNum;
					staticObjDesc.iLevelIndex = LEVEL_LOGO;

					if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOGO, TEXT("Layer_Map"), TEXT("Prototype_GameObject_StaticObj"), &staticObjDesc)))
						return E_FAIL;

					vMinPos.x = min(vMinPos.x, pDesc.vPosition.x);
					vMinPos.y = min(vMinPos.y, pDesc.vPosition.y);
					vMinPos.z = min(vMinPos.z, pDesc.vPosition.z);
					vMaxPos.x = max(vMaxPos.x, pDesc.vPosition.x);
					vMaxPos.y = max(vMaxPos.y, pDesc.vPosition.y);
					vMaxPos.z = max(vMaxPos.z, pDesc.vPosition.z);
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
