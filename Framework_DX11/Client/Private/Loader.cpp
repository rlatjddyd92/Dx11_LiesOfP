#include "stdafx.h"
#include "..\Public\Loader.h"
#include "ThreadPool.h"

#include "Sky.h"
#include "Player.h"
#include "Weapon.h"
#include "Monster.h"
#include "Terrain.h"
#include "ForkLift.h"
#include "FreeCamera.h"
#include "BackGround.h"
#include "Body_Player.h"
#include "Particle_Snow.h"

#include "GameInstance.h"

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
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	/* For. Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

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

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	/* For. Prototype_Component_Texture_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Grass_%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Brush*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Mask */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.bmp"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;


	/* For. Prototype_Component_Texture_Particle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Explosion */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));
	/* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	CVIBuffer_Instancing::INSTANCE_DESC			ParticleDesc{};
	/* For. Prototype_Component_VIBuffer_Particle_Explosion */
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);
	ParticleDesc.iNumInstance = 1000;
	ParticleDesc.vCenter = _float3(0.f, 0.f, 0.f);

	ParticleDesc.vRange = _float3(5.f, 5.f, 5.f);
	ParticleDesc.vExceptRange = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSize = _float2(0.1f, 0.2f);
	ParticleDesc.vSpeed = _float2(1.f, 2.f);
	ParticleDesc.vLifeTime = _float2(5.f, 10.f);
	ParticleDesc.vMinColor = _float4(1.0f, 0.271f, 0.0f, 1.f);
	ParticleDesc.vMaxColor = _float4(1.0f, 0.271f, 0.0f, 1.f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Snow"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션을(를) 로딩중입니다."));
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.dat")))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));
	/* For. Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxAnimModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxCubeTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX::Elements, VTXCUBETEX::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE::Elements, VTXRECTINSTANCE::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("콜라이더을(를) 로딩중입니다."));

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	/* For. Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Body_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),
		CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_FreeCamera */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For. Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_ForkLift */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"),
		CForkLift::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Particle_Snow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Snow"),
		CParticle_Snow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_isFinished_Main = true;

	return S_OK;
}


HRESULT CLoader::Ready_Resources_For_GamePlayLevel_Map0()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For. Prototype_Component_Model_Fiona*/
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/ModelData/Anim/PlayerExample/PlayerExample.dat", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_ForkLift*/
	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/ModelData/NonAnim/Example/Example.dat", PreTransformMatrix, true))))
		return E_FAIL;

	m_isFinished_Map0 = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlayLevel_Map1()
{

	m_isFinished_Map1 = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlayLevel_Monster()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	m_isFinished_Monster = true;

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
