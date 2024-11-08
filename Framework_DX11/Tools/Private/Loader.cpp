#include "stdafx.h"
#include "Loader.h"
#include <functional>
#include <io.h>

#include "Terrain.h"
#include "BackGround.h"
#include "GameInstance.h"
#include "ThreadPool.h"

#include "AnimModel.h"
#include "NonAnimModel.h"
#include "FreeCamera.h"

#include "TargetBall.h"

#include "Particle_Effect.h"
#include "Texture_Effect.h"

#include "Controller_EffectTool.h"
#include "Controller_AnimationTool.h"

#pragma comment(lib, "ole32.lib")	// 지우지 마세요

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
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

	//CThreadPool::Get_Instance()->Initialize();
	//CThreadPool::Get_Instance()->Start_Work();

	InitializeCriticalSection(&m_CriticalSection_Main);
	m_hThread_Main = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread_Main)
		return E_FAIL;

	InitializeCriticalSection(&m_CriticalSection_Map0);
	m_hThread_Map0 = (HANDLE)_beginthreadex(nullptr, 0, LoadingMap0, this, 0, nullptr);
	if (0 == m_hThread_Map0)
		return E_FAIL;

	InitializeCriticalSection(&m_CriticalSection_Map1);
	m_hThread_Map1 = (HANDLE)_beginthreadex(nullptr, 0, LoadingMap1, this, 0, nullptr);
	if (0 == m_hThread_Map1)
		return E_FAIL;

	InitializeCriticalSection(&m_CriticalSection_Monster);
	m_hThread_Monster = (HANDLE)_beginthreadex(nullptr, 0, LoadingMonster, this, 0, nullptr);
	if (0 == m_hThread_Monster)
		return E_FAIL;

	if (LEVEL_LOGO == m_eNextLevelID)
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
	case LEVEL_TOOL:
		hr = Ready_Resources_For_ToolLevel();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection_Main);


	return hr;
}

HRESULT CLoader::LoadingMapModel0()
{
	//EnterCriticalSection(&m_CriticalSection_Map0);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_TOOL:
		hr = Ready_Resources_For_ToolLevel_Map0();
		break;
	default:
		hr = S_OK;
	}

	//if (hr == E_FAIL)
	//	MSG_BOX(TEXT("맵 로드0 에서 터짐"));

	//LeaveCriticalSection(&m_CriticalSection_Map0);

	return hr;
}

HRESULT CLoader::LoadingMapModel1()
{
	EnterCriticalSection(&m_CriticalSection_Map1);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_TOOL:
		hr = Ready_Resources_For_ToolLevel_Map1();
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
	case LEVEL_TOOL:
		hr = Ready_Resources_For_ToolLevel_Monster();
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
	if (isFinished())
		lstrcpy(m_szLoadingText, TEXT("진짜로 로딩이 완료되었습니다."));

	SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Ready_Resources_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	// 텍스쳐를 비동기적으로 로딩하는 작업 추가
	auto textureFuture = CThreadPool::Get_Instance()->Add_Work([this]() {
		return m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2));
		});
	return textureFuture.get();

	///* For. Prototype_Component_Texture_BackGround */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
	//	return E_FAIL;	

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	/* For. Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_isFinished_Main = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_ToolLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	/* For. Prototype_Component_Texture_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.bmp"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Snow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

#pragma region PARTICLE
	/* For. Prototype_Component_Texture_Particle_Spark */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Particle_Spark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Particle_Spark.dds"), 1))))
		return E_FAIL;
	CController_EffectTool::Get_Instance()->Add_Particle_ProtytypeTag(TEXT("Prototype_Component_Texture_Particle_Spark"));

	/* For. Prototype_Component_Texture_Particle_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_Particle_Circle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Particle_Circle.dds"), 1))))
		return E_FAIL;
	CController_EffectTool::Get_Instance()->Add_Particle_ProtytypeTag(TEXT("Prototype_Component_Texture_Particle_Circle"));
#pragma endregion

#pragma region EFFECT
	/* For. Prototype_Component_Texture_Particle_Flare */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_TE_Flare"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_Flare_01_C_HJS.dds"), 1))))
		return E_FAIL;
	CController_EffectTool::Get_Instance()->Add_TE_ProtytypeTag(TEXT("Prototype_Component_Texture_TE_Flare"));

	/* For. Prototype_Component_Texture_Particle_Ring */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_TE_Ring"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_Ring_04_C_KMH.dds"), 1))))
		return E_FAIL;
	CController_EffectTool::Get_Instance()->Add_TE_ProtytypeTag(TEXT("Prototype_Component_Texture_TE_Ring"));

	// 테스트용. 나중에 링이랑 합칠거임.
	/* For. Prototype_Component_Texture_Particle_Spread */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_TE_Spread"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_Ring_08_C_HJS.dds"), 1))))
		return E_FAIL;
	CController_EffectTool::Get_Instance()->Add_TE_ProtytypeTag(TEXT("Prototype_Component_Texture_TE_Spread"));

	// 테스트용. 나중에 플레어랑 합칠거임.
/* For. Prototype_Component_Texture_Particle_Spread */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Texture_TE_LensFlare"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_LensFlare_01_C_KMH.dds"), 1))))
		return E_FAIL;
	CController_EffectTool::Get_Instance()->Add_TE_ProtytypeTag(TEXT("Prototype_Component_Texture_TE_LensFlare"));

#pragma endregion


	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));
	/* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CVIBuffer_Instancing::INSTANCE_DESC ParticleDesc = {};
	/* For. Prototype_Component_VIBuffer_Point_Instance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationX(XMConvertToRadians(90.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Model_Test"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Map/Structure/SM_Monastery_Deco_01.dat", PreTransformMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩중입니다."));
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));
	/* For. Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxAnimModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_Effect_Texture */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_Component_Shader_Effect_Texture"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Effect_Texture.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	/* For. Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NonAnim"),
		CNonAnimModel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Anim"),
		CAnimModel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FreeCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_TargetBall */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TargetBall"),
		CTargetBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region EFFECT
	/* For. Prototype_GameObject_Particle_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Effect"),
		CParticle_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Texture_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Texture_Effect"),
		CTexture_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
	m_isFinished_Main = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_ToolLevel_Map0()
{
	_matrix      PreTransformMatrix = XMMatrixIdentity();

	CModel* pModel = { nullptr };
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(270.f));
	
	CController_AnimationTool::Get_Instance()->SetUp_Lump(2);

	//if (false)
	//{
	//	//"../Bin/ModelData/Anim/PlayerExample/PlayerExample.dat"	일반 바이너리
	//	pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/PlayerExample/PlayerExample.dat", PreTransformMatrix, false, CController_AnimationTool::Get_Instance()->Get_StructStack(0));
	//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_AnimModel_Test"),
	//		pModel)))
	//		return E_FAIL;
	//}
	//else
	{
		//"../Bin/ModelData/Anim/CreatedBinFiles/Test.dat"			추가 조정 바이너리
		pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/CreatedBinFiles/Test.dat", PreTransformMatrix, true);
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_AnimModel_Test"),
			pModel)))
			return E_FAIL;
		m_pGameInstance->Add_ModelPrototype(LEVEL_TOOL, ("Prototype_AnimModel_Test"), pModel);
	}


	pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/Monster/CarcassTail/CarcassTail.dat", PreTransformMatrix, false, CController_AnimationTool::Get_Instance()->Get_StructStack(1));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Prototype_AnimModel_Test2"),
		pModel)))
		return E_FAIL;
	m_pGameInstance->Add_ModelPrototype(LEVEL_TOOL, ("Prototype_AnimModel_Test2"), pModel);



	//// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	//_finddata_t fd;

	//// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	//intptr_t handle = _findfirst("../Bin/ModelData/NonAnim/Map/Structure/*", &fd);

	//if (handle == -1)
	//	return E_FAIL;

	//int iResult = 0;

	//char szCurPath[128] = "../Bin/ModelData/NonAnim/Map/Structure/";    // 상대 경로
	//char szFullPath[128] = "";

	//_wstring strPrototype = TEXT("Part");
	//_uint iNum = 0;

	//while (iResult != -1)
	//{
	//	strcpy_s(szFullPath, szCurPath);
	//	strcat_s(szFullPath, fd.name);

	//	_char szFileName[MAX_PATH] = "";
	//	_char szExt[MAX_PATH] = "";
	//	_splitpath_s(szFullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

	//	if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
	//		|| strcmp(szExt, ".dat"))
	//	{
	//		iResult = _findnext(handle, &fd);
	//		continue;
	//	}

	//	string strFileName = szFileName;
	//	_wstring strPrototypeName;

	//	strPrototypeName.assign(strFileName.begin(), strFileName.end());
	//	wprintf(strPrototypeName.c_str());

	//	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) ;

	//	if (strPrototypeName == TEXT("SM_Monstery_Machine_01A"))
	//		int a = 0;
	//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, strPrototypeName,
	//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath, PreTransformMatrix))))
	//		return E_FAIL;

	//	//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
	//	iResult = _findnext(handle, &fd);
	//}

	m_isFinished_Map0 = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_ToolLevel_Map1()
{
	_matrix      PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationX(XMConvertToRadians(90.0f));

	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd;

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst("../Bin/ModelData/NonAnim/Map/Interior/*", &fd);

	//if (handle == -1)
	//	return E_FAIL;

	int iResult = 0;

	//char szCurPath[128] = "../Bin/ModelData/NonAnim/Map/Interior/";    // 상대 경로
	//char szFullPath[128] = "";

	//_wstring strPrototype = TEXT("");

	//while (iResult != -1)
	//{
	//	strcpy_s(szFullPath, szCurPath);
	//	strcat_s(szFullPath, fd.name);

	//	_char szFileName[MAX_PATH] = "";
	//	_char szExt[MAX_PATH] = "";
	//	_splitpath_s(szFullPath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

	//	if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")
	//		|| strcmp(szExt, ".dat"))
	//	{
	//		iResult = _findnext(handle, &fd);
	//		continue;
	//	}

	//	string strFileName = szFileName;
	//	_wstring strPrototypeName;

	//	strPrototypeName.assign(strFileName.begin(), strFileName.end());
	//	wprintf(strPrototypeName.c_str());

	//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, strPrototypeName,
	//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath, PreTransformMatrix))))
	//		return E_FAIL;

	//	//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
	//	iResult = _findnext(handle, &fd);
	//}

	handle = _findfirst("../Bin/ModelData/NonAnim/Map/Line/*", &fd);

	if (handle == -1)
		return E_FAIL;

	char szCurPath2[128] = "../Bin/ModelData/NonAnim/Map/Line/";    // 상대 경로
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
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath2, PreTransformMatrix))))
			return E_FAIL;

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	handle = _findfirst("../Bin/ModelData/NonAnim/Map/Temp/*", &fd);

	if (handle == -1)
		return E_FAIL;

	char szCurPath3[128] = "../Bin/ModelData/NonAnim/Map/Temp/";    // 상대 경로
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
		PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, strPrototypeName,
			CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, szFullPath3, PreTransformMatrix))))
			return E_FAIL;

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	PreTransformMatrix = XMMatrixScaling(0.0003f, 0.0003f, 0.0003f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_TOOL, TEXT("Light"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Map/Light.dat", PreTransformMatrix))))
		return E_FAIL;

	m_isFinished_Map1 = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_ToolLevel_Monster()
{
	m_isFinished_Monster = true;

	return S_OK;
}

void CLoader::aaa()
{
	int a = 0;
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
