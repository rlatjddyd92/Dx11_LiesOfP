#include "stdafx.h"
#include "..\Public\Loader.h"
#include "ThreadPool.h"
#include <io.h>

#include "ColliderObject.h"

#include "Sky.h"
#include "Player.h"
#include "Terrain.h"
#include "FreeCamera.h"
#include "PlayerCamera.h"
#include "BackGround.h"
#include "StaticObj.h"
#include "NavDataObj.h"
#include "CutScene.h"

#pragma region MONSTER
#include "Monster.h"
#include "CarcassBigA.h"
#include "CarcassTail.h"
#include "CarcassNormal.h"
#include "CurruptedStrongArm_Puppet.h"
#include "RebornerBigA.h"
#include "Weapon_RebornerMale_Gun.h"
#include "RebornerMale.h"
#include "Weapon_RebornerBigA_Stick.h"
#include "SimonManus.h"

#include "SpotEffect.h"
#include "FollowedEffect.h"

#include "AObj_Bullet.h"

#include "AObj_StampBlast.h"
#include "AObj_ChargeSwing.h"
#include "AObj_GoldBall.h"
#include "AObj_Thunder.h"
#include "AObj_ThunderCalling.h"
#include "AObj_LightningSpear.h"
#include "AObj_LightningBall.h"
#include "AObj_BlackBall.h"
#include "AObj_GodHands.h"
#include "GH_Creater.h"
#include "AObj_Stomp.h"
#include "AObj_Wave.h"

#include "Raxasia.h"
#include "AObj_ThunderBlast.h"
#include "AObj_ThunderLanding.h"
#include "AObj_ThunderMark.h"
#include "AObj_ThunderStampMark.h"
#include "AObj_ThunderSpread.h"
#include "AObj_ThunderStamp.h"
#include "AObj_ThunderWave.h"
#include "AObj_ThunderBolt.h"

#include "Weapon_Raxasia_P1_Sword.h"
#include "Weapon_Raxasia_P2_Sword.h"
#include "Weapon_Raxasia_P1_Shield.h"
#include "Weapon_Raxasia_P2_Shield.h"

#include "SimonManus_EnvHand.h"
#include "SimonManus_CutScene_GodHand.h"

#include "Raxasia_Sword_CutScene.h"
#include "Raxasia_Helmet_CutScene.h"

#include "Monster_Training01.h"
#include "Monster_Training02.h"
#pragma endregion

#pragma region EFFECT
#include "Effect_Manager.h"
#pragma endregion

#pragma region OBJECT
#include "Stargazer.h"
#include "Lift_Controller.h"
#include "Lift_Door.h"
#include "Lift_Floor.h"
#include "WallDeco.h"
#include "Deco_Collider.h"
#include "TowerDoor.h"
#include "LastDoor.h"
#include "TreasureBox.h"
#include "Ladder.h"
#include "SteppingStone.h"
#include "Weapon_Rapier.h"
#include "Weapon_FlameSword.h"
#include "Weapon_Scissor.h"
#include "Weapon_Scissor_Handle.h"
#include "Weapon_Scissor_Blade.h"
#include "Weapon_PlayerArm.h"
#include "Weapon_SimonManus_Hammer.h"
#include "Sophia.h"
#include "Item_Throw.h"
#include "TorchDeck.h"
#include "FirePot.h"
#include "SteelHeart.h"
#pragma endregion

#include "Machine_EffectObj.h"
#include "Decal.h"

#include "GameInstance.h"
#include "GameInterface_Controller.h"

CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = static_cast<CLoader*>(pArg);

	if(FAILED(pLoader->Loading()))
		return 1;
		
	return 0;
}

_uint APIENTRY LoadingMap0(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->LoadingMapModel0()))
		return 1;

	return 0;
}

_uint APIENTRY LoadingMap1(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->LoadingMapModel1()))
		return 1;

	return 0;
}

_uint APIENTRY LoadingMonster(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->LoadingMonsterModel1()))
		return 1;

	return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	// 나중에 쓸거면 엔진으로 만들기
	//m_pThreadPool = CThreadPool::Get_Instance();

	//m_pThreadPool->Initialize();
	//m_pThreadPool->Start_Work();
	
	InitializeCriticalSection(&m_CriticalSection_Main);
	m_hThread_Main = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread_Main)
		return E_FAIL;

	InitializeCriticalSection(&m_CriticalSection_Map0);
	m_hThread_Map0 = (HANDLE)_beginthreadex(nullptr, 0, LoadingMap0, this, 0, nullptr);
	if (0 == m_hThread_Map0)
		return E_FAIL;
	m_isFinished_Map0 = true;

	InitializeCriticalSection(&m_CriticalSection_Map1);
	m_hThread_Map1 = (HANDLE)_beginthreadex(nullptr, 0, LoadingMap1, this, 0, nullptr);
	if (0 == m_hThread_Map1)
		return E_FAIL;

	InitializeCriticalSection(&m_CriticalSection_Monster);
	m_hThread_Monster = (HANDLE)_beginthreadex(nullptr, 0, LoadingMonster, this, 0, nullptr);
	if (0 == m_hThread_Monster)
		return E_FAIL;

	if (m_eNextLevelID == LEVEL_LOGO)
	{
		m_isFinished_Map0 = true;
		m_isFinished_Map1 = true;
		m_isFinished_Monster = true;
	}

	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection_Main);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Ready_Resources_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Ready_Resources_For_GamePlayLevel();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection_Main);
	

	return hr;
}

HRESULT CLoader::LoadingMapModel0()
{
	EnterCriticalSection(&m_CriticalSection_Map0);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_GAMEPLAY:
		hr = Ready_Resources_For_GamePlayLevel_Map0();
		break;
	default:
		hr = S_OK;
	}

	if (hr == E_FAIL)
		MSG_BOX(TEXT("맵 로드0 에서 터짐"));

	LeaveCriticalSection(&m_CriticalSection_Map0);

	return hr;
}

HRESULT CLoader::LoadingMapModel1()
{
	EnterCriticalSection(&m_CriticalSection_Map1);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_GAMEPLAY:
		hr = Ready_Resources_For_GamePlayLevel_Map1();
		break;
	default:
		hr = S_OK;
	}

	if (hr == E_FAIL)
		MSG_BOX(TEXT("맵 로드1 에서 터짐"));

	LeaveCriticalSection(&m_CriticalSection_Map1);

	return hr;
}

HRESULT CLoader::LoadingMonsterModel1()
{
	EnterCriticalSection(&m_CriticalSection_Monster);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_GAMEPLAY:
		hr = Ready_Resources_For_GamePlayLevel_Monster();
		break;
	default:
		hr = S_OK;
	}

	if (hr == E_FAIL)
		MSG_BOX(TEXT("몬스터 로드0 에서 터짐"));

	LeaveCriticalSection(&m_CriticalSection_Monster);

	return hr;
}

void CLoader::Draw_LoadingText()
{
	if(isFinished())
		lstrcpy(m_szLoadingText, TEXT("로딩 완료"));

	SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Ready_Resources_For_LogoLevel()
{
	if (FAILED(Ready_Prototype()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	/* For. Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	_matrix      PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_SteellHeart"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/SteelHeart/SteelHeart.dat", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Lift_Door"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/InteractObj/SK_NewTown_Lift_01_Door.dat", PreTransformMatrix))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));
	m_pGameInstance->LoadSoundFile("BGM");
	m_pGameInstance->LoadSoundFile("Enviroment");
	m_pGameInstance->LoadSoundFile("Player");
	m_pGameInstance->LoadSoundFile("Player/Rapier");
	m_pGameInstance->LoadSoundFile("Player/Flame");
	m_pGameInstance->LoadSoundFile("Player/Scissor");
	m_pGameInstance->LoadSoundFile("Player/Guard");
	m_pGameInstance->LoadSoundFile("Player/Interact");
	m_pGameInstance->LoadSoundFile("Player/Hit");

	m_pGameInstance->LoadSoundFile("UI");

	m_pGameInstance->LoadSoundFile("Monster/Effect");
	m_pGameInstance->LoadSoundFile("Monster/SimonManus");

	m_pGameInstance->LoadSoundFile("Scenario/Object");
	m_pGameInstance->LoadSoundFile("Scenario/CutScene");

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	/* For. Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_isFinished_Main = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlayLevel()
{
	///* For. Prototype_Component_Texture_Terrain*/
	//for (_uint i = 0; i < 200; ++i)
	//{
	//	_wstring ex = TEXT("Prototype_Component_Texture_Terrain");
	//	_wstring num = to_wstring(i);
	//	ex = ex + num;
	//	auto textureFuture = CThreadPool::Get_Instance()->Add_Work([this, ex]() {
	//		return m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, ex,
	//		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2));
	//		});
	//	if (textureFuture.get() == E_FAIL)
	//		return textureFuture.get();
	//}

	//_matrix      PreTransformMatrix = XMMatrixIdentity();
	//PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SteellHeart"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/SteelHeart/SteelHeart.dat", PreTransformMatrix))))
	//	return E_FAIL;

	// 24-11-26 김성용
	// 로딩 화면 조정용 내용 추가 
	GET_GAMEINTERFACE->Set_Loading_Status(TEXT("이펙트 준비 중"), 0.2f);

	lstrcpy(m_szLoadingText, TEXT("이펙트 매니저를 로딩중입니다."));
	if(FAILED(CEffect_Manager::Get_Instance()->Initialize(m_pDevice, m_pContext, 
		TEXT("../../Tools/Bin/DataFiles/Effect"), TEXT("../Bin/Resources/Textures/Effect"), TEXT("../Bin/ModelData/NonAnim/Effect"))))
		return E_FAIL;

	GET_GAMEINTERFACE->Set_Loading_Status(TEXT("이미지 준비 중"), 0.4f);

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	/* For. Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 6))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));

	GET_GAMEINTERFACE->Set_Loading_Status(TEXT("플레이어 준비 중"), 0.5f);

	if (FAILED(Ready_Resources_For_Player()))
		return E_FAIL;

	GET_GAMEINTERFACE->Set_Loading_Status(TEXT("시몬 마누스가 다가오는 중"), 0.6f);

	GET_GAMEINTERFACE->Set_Loading_Status(TEXT("무기와 장비를 준비하는 중"), 0.7f);
	
	if (FAILED(Ready_Resources_For_Obj()))
		return E_FAIL;

	GET_GAMEINTERFACE->Set_Loading_Status(TEXT("거짓의 세계를 만드는 중"), 0.8f);

	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩중입니다."));
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Nav_Data.dat")))))
		return E_FAIL;

	GET_GAMEINTERFACE->Set_Loading_Status(TEXT("소리 준비 중"), 0.9f);

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	GET_GAMEINTERFACE->Set_Loading_Status(TEXT("준비 완료"), 1.f);
	
	m_isFinished_Main = true;

	return S_OK;
}


HRESULT CLoader::Ready_Resources_For_GamePlayLevel_Map0()
{
	_matrix      PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);

	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	//if (handle == -1)
	//	return E_FAIL;


	int iResult = 0;

	_wstring strPrototype = TEXT("");

#pragma region ETC Cathedral
	intptr_t handle = _findfirst("../Bin/ModelData/NonAnim/Map/Etc/Cathedral/*", &fd);

	if (handle == -1)
		return E_FAIL;

	char szCurPath0[128] = "../Bin/ModelData/NonAnim/Map/Etc/Cathedral/";    // 상대 경로
	char szFullPath0[128] = "";

	iResult = 0;

	while (iResult != -1)
	{
		strcpy_s(szFullPath0, szCurPath0);
		strcat_s(szFullPath0, fd.name);

		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szFullPath0, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
			|| strcmp(szExt, ".dat"))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		string strFileName = szFileName;
		_wstring strPrototypeName;

		strPrototypeName.assign(strFileName.begin(), strFileName.end());
		wprintf(strPrototypeName.c_str());

		PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath0, PreTransformMatrix))))
			return E_FAIL;

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}
#pragma endregion

#pragma region ETC Interior
	handle = _findfirst("../Bin/ModelData/NonAnim/Map/Etc/Interior/*", &fd);

	if (handle == -1)
		return E_FAIL;

	char szCurPath1[128] = "../Bin/ModelData/NonAnim/Map/Etc/Interior/";    // 상대 경로
	char szFullPath1[128] = "";

	iResult = 0;

	while (iResult != -1)
	{
		strcpy_s(szFullPath1, szCurPath1);
		strcat_s(szFullPath1, fd.name);

		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szFullPath1, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
			|| strcmp(szExt, ".dat"))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		string strFileName = szFileName;
		_wstring strPrototypeName;

		strPrototypeName.assign(strFileName.begin(), strFileName.end());
		wprintf(strPrototypeName.c_str());

		PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath1, PreTransformMatrix))))
			return E_FAIL;

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}
#pragma endregion

#pragma region ETC Furniture
	handle = _findfirst("../Bin/ModelData/NonAnim/Map/Etc/Furniture/*", &fd);

	if (handle == -1)
		return E_FAIL;

	char szCurPath2[128] = "../Bin/ModelData/NonAnim/Map/Etc/Furniture/";    // 상대 경로
	char szFullPath2[128] = "";

	iResult = 0;

	while (iResult != -1)
	{
		strcpy_s(szFullPath2, szCurPath2);
		strcat_s(szFullPath2, fd.name);

		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szFullPath2, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
			|| strcmp(szExt, ".dat"))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		string strFileName = szFileName;
		_wstring strPrototypeName;

		strPrototypeName.assign(strFileName.begin(), strFileName.end());
		wprintf(strPrototypeName.c_str());

		PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath2, PreTransformMatrix))))
			return E_FAIL;

		iResult = _findnext(handle, &fd);
	}
#pragma endregion

#pragma region ETC WoodStr
	handle = _findfirst("../Bin/ModelData/NonAnim/Map/Etc/WoodStr/*", &fd);

	if (handle == -1)
		return E_FAIL;

	char szCurPath3[128] = "../Bin/ModelData/NonAnim/Map/Etc/WoodStr/";    // 상대 경로
	char szFullPath3[128] = "";

	iResult = 0;

	while (iResult != -1)
	{
		strcpy_s(szFullPath3, szCurPath3);
		strcat_s(szFullPath3, fd.name);

		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szFullPath3, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
			|| strcmp(szExt, ".dat"))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		string strFileName = szFileName;
		_wstring strPrototypeName;

		strPrototypeName.assign(strFileName.begin(), strFileName.end());
		wprintf(strPrototypeName.c_str());

		PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath3, PreTransformMatrix))))
			return E_FAIL;

		iResult = _findnext(handle, &fd);
	}
#pragma endregion

#pragma region Decal Textures
	 handle = _findfirst("../Bin/Resources/Textures/Decal/dds/ENV_Decals/*", &fd);

	if (handle == -1)
		return E_FAIL;

	char szCurPath4[128] = "../Bin/Resources/Textures/Decal/dds/ENV_Decals/";    // 상대 경로
	char szFullPath4[128] = "";

	iResult = 0;

	while (iResult != -1)
	{
		strcpy_s(szFullPath4, szCurPath4);
		strcat_s(szFullPath4, fd.name);

		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szFullPath4, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
			|| strcmp(szExt, ".dds"))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		string strFileName = szFileName;
		_wstring strPrototypeName;

		strPrototypeName.assign(strFileName.begin(), strFileName.end());
		wprintf(strPrototypeName.c_str());	

		_tchar path[MAX_PATH];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szFullPath4, MAX_PATH, path, MAX_PATH);

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeName,
			CTexture::Create(m_pDevice, m_pContext, path, 1))))
			return E_FAIL;

		iResult = _findnext(handle, &fd);
	}
#pragma endregion

	PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Sophia_Stoned"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Map/Etc/Sophia/Sophia_Stoned.dat", PreTransformMatrix))))
		return E_FAIL;	
	
	PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("SM_LightProp_Chandelier_05"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Map/Etc/Light/SM_LightProp_Chandelier_05.dat", PreTransformMatrix))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("SM_LightProp_SpotLight_02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Map/Etc/Light/SM_LightProp_SpotLight_02.dat", PreTransformMatrix))))
		return E_FAIL;

	m_isFinished_Map0 = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlayLevel_Map1()
{
	_matrix      PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);

	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst("../Bin/ModelData/NonAnim/Map/Temp/*", &fd);

	//if (handle == -1)
	//	return E_FAIL;

	int iResult = 0;

	_wstring strPrototype = TEXT("");

	if (handle == -1)
		return E_FAIL;

#pragma region Temp
	char szCurPath0[128] = "../Bin/ModelData/NonAnim/Map/Temp/";    // 상대 경로
	char szFullPath0[128] = "";

	iResult = 0;

	while (iResult != -1)
	{
		strcpy_s(szFullPath0, szCurPath0);
		strcat_s(szFullPath0, fd.name);

		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szFullPath0, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
			|| strcmp(szExt, ".dat"))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		string strFileName = szFileName;
		_wstring strPrototypeName;

		strPrototypeName.assign(strFileName.begin(), strFileName.end());
		wprintf(strPrototypeName.c_str());

		PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath0, PreTransformMatrix))))
			return E_FAIL;

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}
#pragma endregion

#pragma region Structure
	handle = _findfirst("../Bin/ModelData/NonAnim/Map/Structure/*", &fd);

	if (handle == -1)
		return E_FAIL;

	iResult = 0;

	char szCurPath1[128] = "../Bin/ModelData/NonAnim/Map/Structure/";    // 상대 경로
	char szFullPath1[128] = "";


	while (iResult != -1)
	{
		strcpy_s(szFullPath1, szCurPath1);
		strcat_s(szFullPath1, fd.name);

		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szFullPath1, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
			|| strcmp(szExt, ".dat"))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		string strFileName = szFileName;
		_wstring strPrototypeName;

		strPrototypeName.assign(strFileName.begin(), strFileName.end());
		wprintf(strPrototypeName.c_str());

		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);

		//if (strPrototypeName == TEXT("SM_Monstery_Machine_01A"))
		//	int a = 0;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath1, PreTransformMatrix))))
			return E_FAIL;

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}
#pragma endregion

#pragma region Interior
	handle = _findfirst("../Bin/ModelData/NonAnim/Map/Interior/*", &fd);

	if (handle == -1)
		return E_FAIL;

	iResult = 0;

	char szCurPath3[128] = "../Bin/ModelData/NonAnim/Map/Interior/";    // 상대 경로
	char szFullPath3[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath3, szCurPath3);
		strcat_s(szFullPath3, fd.name);

		_char szFileName[MAX_PATH] = "";
		_char szExt[MAX_PATH] = "";
		_splitpath_s(szFullPath3, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
			|| strcmp(szExt, ".dat"))
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		string strFileName = szFileName;
		_wstring strPrototypeName;

		strPrototypeName.assign(strFileName.begin(), strFileName.end());
		wprintf(strPrototypeName.c_str());

		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);

		//if (strPrototypeName == TEXT("SM_Monstery_Machine_01A"))
		//	int a = 0;
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath3, PreTransformMatrix))))
			return E_FAIL;

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}
#pragma endregion

	m_isFinished_Map1 = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlayLevel_Monster()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();


	if (FAILED(Ready_Resources_For_Monster()))
		return E_FAIL;

	m_isFinished_Monster = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Player()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* Prototype_Component_Model_Player */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/Player.dat", PreTransformMatrix, true))))
		return E_FAIL;

	/* Prototype_Component_Model_Rapier */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rapier"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/Weapon/Rapier.dat", PreTransformMatrix, false))))
		return E_FAIL;

	/* Prototype_Component_Model_FlameSword */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FlameSword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/Weapon/FlameSword.dat", PreTransformMatrix, false))))
		return E_FAIL;

	/* Prototype_Component_Model_Scissor_Combine */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Scissor_Combine"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/Weapon/Scissor_Combine.dat", PreTransformMatrix, false))))
		return E_FAIL;

	/* Prototype_Component_Model_Scissor_Left_Hnd */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationZ(XMConvertToRadians(-180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Scissor_Left_Hnd"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/Weapon/Scissor_Left_Handle.dat", PreTransformMatrix, false))))
		return E_FAIL;

	/* Prototype_Component_Model_Scissor_Left_Bld */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationZ(XMConvertToRadians(-180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Scissor_Left_Bld"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/Weapon/Scissor_Left_Blade.dat", PreTransformMatrix, false))))
		return E_FAIL;

	/* Prototype_Component_Model_Scissor_Right_Hnd */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixRotationZ(XMConvertToRadians(-180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Scissor_Right_Hnd"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/Weapon/Scissor_Right_Handle.dat", PreTransformMatrix, false))))
		return E_FAIL;

	/* Prototype_Component_Model_Scissor_Right_Bld */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(270.f)) * XMMatrixRotationZ(XMConvertToRadians(-180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Scissor_Right_Bld"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/Weapon/Scissor_Right_Blade.dat", PreTransformMatrix, false))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Monster()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	//Prototype_Component_Model_CarcassTail
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CarcassTail"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/CarcassTail.dat", PreTransformMatrix, true))))
		return E_FAIL;

	//Prototype_Component_Model_CarcassNormal
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CarcassNormal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/CarcassNormal.dat", PreTransformMatrix, true))))
		return E_FAIL;

	//Prototype_Component_Model_CarcassBigA
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CarcassBigA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/CarcassBigA.dat", PreTransformMatrix, true))))
		return E_FAIL;

	//Prototype_Component_Model_CurruptedStrongArm
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Horesman"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/Horesman.dat", PreTransformMatrix, true))))
		return E_FAIL;

	//Prototype_Component_Model_RebornerMale
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RebornerMale"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/RebornerMale.dat", PreTransformMatrix, true))))
		return E_FAIL;

	//Prototype_Component_Model_Weapon_RebornerMale_Gun
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationZ(XMConvertToRadians(180.0f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_RebornerMale_Gun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/RebornerMale/RebornerMale_Weapon.dat", PreTransformMatrix, false))))
		return E_FAIL;

	//Prototype_Component_Model_RebornerBigA
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RebornerBigA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/RebornerBigA.dat", PreTransformMatrix, true))))
		return E_FAIL;

	//Prototype_Component_Model_Weapon_RebornerBigA_Stick
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationZ(XMConvertToRadians(90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_RebornerBigA_Stick"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/RebornerBigA/RebornerBigA_Weapon.dat", PreTransformMatrix, false))))
		return E_FAIL;

	//Prototype_Component_Model_SimonManus
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManusP1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/SimonManus_P1.dat", PreTransformMatrix, true))))
		return E_FAIL;

	//Prototype_Component_Model_SimonManusP2
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManusP2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/SimonManus_P2.dat", PreTransformMatrix, true))))
		return E_FAIL;

	//Prototype_Component_Model_Weapon_SimonManus_Hammer
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_SimonManus_Hammer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/SimonManus_Weapon.dat", PreTransformMatrix, true))))
		return E_FAIL;

	/* For. Prototype_AnimModel_SimonManus_GodHand */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManus_GodHand"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/SimonManus_GodHand.dat", PreTransformMatrix, true))))
		return E_FAIL;

	//Prototype_Component_Model_RaxasiaP1
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RaxasiaP1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/Raxasia_P1.dat", PreTransformMatrix, true))))
		return E_FAIL;

	//Prototype_Component_Model_RaxasiaP2
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RaxasiaP2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/Raxasia_P2.dat", PreTransformMatrix, true))))
		return E_FAIL;

	//Prototype_Component_Model_RaxasiaP1_Sword
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RaxasiaP1_Sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Boss/Raxasia/Raxasia_Sword01.dat", PreTransformMatrix, false))))
		return E_FAIL;

	//Prototype_Component_Model_RaxasiaP1_Shield
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RaxasiaP1_Shield"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Boss/Raxasia/Raxasia_Shield01.dat", PreTransformMatrix, false))))
		return E_FAIL;

	//Prototype_Component_Model_RaxasiaP2_Sword
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RaxasiaP2_Sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Boss/Raxasia/Raxasia_Sword02.dat", PreTransformMatrix, false))))
		return E_FAIL;

	//Prototype_Component_Model_RaxasiaP2_Shield
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RaxasiaP2_Shield"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Boss/Raxasia/Raxasia_Shield02.dat", PreTransformMatrix, false))))
		return E_FAIL;


#pragma region 훈련 로봇
	//Prototype_Component_Model_Training01
	PreTransformMatrix = XMMatrixScaling(0.011f, 0.011f, 0.011f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Training01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Training/Training01.dat", PreTransformMatrix, false))))
		return E_FAIL;

	//Prototype_Component_Model_Training02
	PreTransformMatrix = XMMatrixScaling(0.011f, 0.011f, 0.011f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Training02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Training/Training02.dat", PreTransformMatrix, false))))
		return E_FAIL;
#pragma endregion

#pragma region 컷신용 모델들
	//Prototype_Component_Model_SimonManus_CutScene_P1
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManus_CutScene_P1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Boss/SimonManus/SimonManus_CutScene_P1.dat", PreTransformMatrix, false))))
		return E_FAIL;
	/*if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManus_CutScene_P1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/SimonManus_CutScene_P1.dat", PreTransformMatrix, true))))
		return E_FAIL;*/

		//Prototype_Component_Model_SimonManus_CutScene_P2
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManus_CutScene_P2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Boss/SimonManus/SimonManus_CutScene_P2.dat", PreTransformMatrix, false))))
		return E_FAIL;

	//Prototype_Component_Model_SimonManus_CutScene_EnvHand
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManus_CutScene_EnvHand"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/SimonManus_CutScene_EnvHand.dat", PreTransformMatrix, true))))
		return E_FAIL;

	//Prototype_Component_Model_SimonManus_CutScene_GodHand
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SimonManus_CutScene_GodHand"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Boss/SimonManus/CutScene_GodHand.dat", PreTransformMatrix, false))))
		return E_FAIL;

	//Prototype_Component_Model_Raxasia_CutScene_P1
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Raxasia_CutScene_P1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/Raxasia_CutScene_P1.dat", PreTransformMatrix, true))))
		return E_FAIL;

	//Prototype_Component_Model_Raxasia_CutScene_P2
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Raxasia_CutScene_P2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/Raxasia_CutScene_P2.dat", PreTransformMatrix, true))))
		return E_FAIL;

	//Prototype_Component_Model_Raxasia_Sword_CutScene
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationX(XMConvertToRadians(215.f)) * XMMatrixRotationY(XMConvertToRadians(-50.f)) * XMMatrixRotationZ(XMConvertToRadians(15.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Raxasia_Sword_CutScene"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Boss/Raxasia/Raxasia_Sword_CutScene.dat", PreTransformMatrix, false))))
		return E_FAIL;

	//Prototype_Component_Model_Raxasia_Helmet_CutScene
	PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Raxasia_Helmet_CutScene"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/Boss/Raxasia/Raxasia_CutScene_Helmet.dat", PreTransformMatrix, false))))
		return E_FAIL;
#pragma endregion

	/* 락사시어 머리카락용 텍스쳐 */

	/* For. Prototype_Component_Texture_RaxasiaDOU */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RaxasiaDOU"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/ModelData/Anim/Monster/Boss/Raxasia/T_CH_MOB_Raxasia_A01_Hair_DOU.dds"), 1))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Obj()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For. Prototype_Component_Model_Stargazer*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Stargazer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/InteractObj/SK_DLV_Stargazer_01.dat", PreTransformMatrix))))
		return E_FAIL;	

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lift_Controller"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/InteractObj/SK_FO_Monastery_Lift_01_Controller.dat", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lift_Door"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/InteractObj/SK_NewTown_Lift_01_Door.dat", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Tower_Door"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/InteractObj/SK_FO_Monastery_TowerDoor_01.dat", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_WallDeco"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/InteractObj/SK_FO_Monastery_WallDeco_01_Scupture04.dat", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("SK_LV_Ladder_MetalWood_01_KSJ"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/InteractObj/SK_LV_Ladder_MetalWood_01_KSJ.dat", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("SK_LV_Ladder_MetalWood_Slide6m_SM_KSJ"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/InteractObj/SK_LV_Ladder_MetalWood_Slide6m_SM_KSJ.dat", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LastDoor"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/InteractObj/SK_FO_Monastery_TheLastDoor_01.dat", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TreasureBox"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/InteractObj/SK_FO_TreasureChest_02_Red.dat", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Stargazer_Broken"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/NonAnim/InteractObj/SK_DLV_Stargazer_Broken_01.dat", PreTransformMatrix))))
		return E_FAIL;

	/* 던지기용 아이템 */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Throw_Battery"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/ThrowItem/Battery.dat", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Throw_Cluster"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/ThrowItem/Cluster.dat", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Throw_Thermite"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/ThrowItem/Thermite.dat", PreTransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Prototype()
{
	/* For. Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region CAMERA
	/* For. Prototype_GameObject_FreeCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_PlayerCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PlayerCamera"),
		CPlayerCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region PLAYER
	/* For. Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon_Rapier */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Rapier"),
		CWeapon_Rapier::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon_FlameSword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_FlameSword"),
		CWeapon_FlameSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon_Scissor */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Scissor"),
		CWeapon_Scissor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon_Scissor_Handle */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Scissor_Handle"),
		CWeapon_Scissor_Handle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon_Scissor_Blade */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Scissor_Blade"),
		CWeapon_Scissor_Blade::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon_Scissor_Blade */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_PlayerArm"),
		CWeapon_PlayerArm::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region MONSTER

	/* For. Prototype_GameObject_CarcassTail */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CarcassTail"),
		CCarcassTail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_CarcassTail */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CarcassNormal"),
		CCarcassNormal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_CarcassBigA */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CarcassBigA"),
		CCarcassBigA::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_CurruptedStrongArm */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CurruptedStrongArm"),
		CCurruptedStrongArm_Puppet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_RebornerMale */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RebornerMale"),
		CRebornerMale::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon_RebornerMale_Gun */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_RebornerMale_Gun"),
		CWeapon_RebornerMale_Gun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Reb_Bullet"),
		CAObj_Bullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For. Prototype_GameObject_RebornerBigA */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RebornerBigA"),
		CRebornerBigA::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon_RebornerBigA_Stick */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_RebornerBigA_Stick"),
		CWeapon_RebornerBigA_Stick::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_SimonManus */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SimonManus"),
		CSimonManus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// 이펙트 있는 공격 오브젝트
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StampBlast"),
		CAObj_StampBlast::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChargeSwing"),
		CAObj_ChargeSwing::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GoldBall"),
		CAObj_GoldBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Wave"),
		CAObj_Wave::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThunderCalling"),
		CAObj_ThunderCalling::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Thunder"),
		CAObj_Thunder::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LightningBall"),
		CAObj_LightningBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LightningSpear"),
		CAObj_LightningSpear::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BlackBall"),
		CAObj_BlackBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GodHands"),
		CAObj_GodHands::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stomp"),
		CAObj_Stomp::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GodHandCreater"),
		CGH_Creater::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon_SimonManus_Hammer */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_SimonManus_Hammer"),
		CWeapon_SimonManus_Hammer::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For. Prototype_GameObject_Raxasia */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Raxasia"),
		CRaxasia::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon__Raxasia_P1_Sword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Raxasia_P1_Sword"),
		CWeapon_Raxasia_P1_Sword::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For. Prototype_GameObject_Weapon__Raxasia_P1_Sword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Raxasia_P2_Sword"),
		CWeapon_Raxasia_P2_Sword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon__Raxasia_P1_Sword */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Raxasia_P1_Shield"),
		CWeapon_Raxasia_P1_Shield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon_Raxasia_P2_Shield */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Raxasia_P2_Shield"),
		CWeapon_Raxasia_P2_Shield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// 이펙트 있는 공격 오브젝트

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThunderBlast"),
		CAObj_ThunderBlast::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThunderLanding"),
		CAObj_ThunderLanding::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThunderMark"),
		CAObj_ThunderMark::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThunderStampMark"),
		CAObj_ThunderStampMark::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThunderSpread"),
		CAObj_ThunderSpread::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThunderStamp"),
		CAObj_ThunderStamp::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThunderWave"),
		CAObj_ThunderWave::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThunderBolt"),
		CAObj_ThunderBolt::Create(m_pDevice, m_pContext))))
		return E_FAIL;





	/* For. Prototype_GameObject_CutScene_SimonManus_GodHand */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CutScene_SimonManus_GodHand"),
		CSimonManus_CutScene_GodHand::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon_Raxasia_Sword_CutScene */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Raxasia_Sword_CutScene"),
		CRaxasia_Sword_CutScene::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon_Raxasia_Helmet_CutScene */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon_Raxasia_Helmet_CutScene"),
		CRaxasia_Helmet_CutScene::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	// 특정 위치에 판정 없이 소환되는 이펙트
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SpotEffect"),
		CSpotEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	// 뼈를, 혹은 위치를 따라가는 이펙트
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FollowedEffect"),
		CFollowedEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_CutScene_SimonManus_EnvHand */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CutScene_SimonManus_EnvHand"),
		CSimonManus_EnvHand::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_CutScene_SimonManus_EnvHand */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Training01"),
		CMonster_Training01::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_CutScene_SimonManus_EnvHand */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Training02"),
		CMonster_Training02::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Collider
	/* For. Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ColliderObj"),
		CColliderObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region Obj
	/* For. Prototype_GameObject_StaticObj */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StaticObj"),
		CStaticObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Stargazer */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Stargazer"),
		CStargazer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_Lift_Controller */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lift_Controller"),
		CLift_Controller::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Lift_Door */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lift_Door"),
		CLift_Door::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Lift_Floor */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lift_Floor"),
		CLift_Floor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_TowerDoor */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TowerDoor"),
		CTowerDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_WallDeco */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WallDeco"),
		CWallDeco::Create(m_pDevice, m_pContext))))
		return E_FAIL;

		/* For. Prototype_GameObject_Deco_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Deco_Collider"),
		CDeco_Collider::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. Prototype_GameObject_TreasureBox */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TreasureBox"),
		CTreasureBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Ladder */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ladder"),
		CLadder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_SteppingStone */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SteppingStone"),
		CSteppingStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_LastDoor */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LastDoor"),
		CLastDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Sophia */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sophia"),
		CSophia::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_ThrowItem */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ThrowItem"),
		CItem_Throw::Create(m_pDevice, m_pContext))))
		return E_FAIL;	
	
	/* For. Prototype_GameObject_TorchDeck */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TorchDeck"),
		CTorchDeck::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FirePot */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FirePot"),
		CFirePot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_SteelHeart */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SteelHeart"),
		CSteelHeart::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

	/* For. Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For. Prototype_GameObject_NavDataObj */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NavDataObj"),
		CNavDataObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_CutScene */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CutScene"),
		CCutScene::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For. Prototype_GameObject_MachineEffect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MachineEffect"),
		CMachine_EffectObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Decal*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Decal"),
		CDecal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread_Main, INFINITE);
	WaitForSingleObject(m_hThread_Map0, INFINITE);
	WaitForSingleObject(m_hThread_Map1, INFINITE);
	WaitForSingleObject(m_hThread_Monster, INFINITE);

	DeleteObject(m_hThread_Main);
	DeleteObject(m_hThread_Map0);
	DeleteObject(m_hThread_Map1);
	DeleteObject(m_hThread_Monster);

	CloseHandle(m_hThread_Main);
	CloseHandle(m_hThread_Map0);
	CloseHandle(m_hThread_Map1);
	CloseHandle(m_hThread_Monster);

	DeleteCriticalSection(&m_CriticalSection_Main);
	DeleteCriticalSection(&m_CriticalSection_Map0);
	DeleteCriticalSection(&m_CriticalSection_Map1);
	DeleteCriticalSection(&m_CriticalSection_Monster);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
