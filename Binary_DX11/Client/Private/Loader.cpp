#include "stdafx.h"
#include "..\Public\Loader.h"

#include "Terrain.h"
#include "BackGround.h"
#include "GameInstance.h"

#include "NonAnimModel.h"
#include "FreeCamera.h"

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

	CLoader*		pLoader = static_cast<CLoader*>(pArg);

	if(FAILED(pLoader->Loading()))
		return 1;
		
	return 0;
}


HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_GAMEPLAY:
		hr = Ready_Resources_For_GamePlayLevel();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	return hr;
}

void CLoader::Draw_LoadingText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Ready_Resources_For_LogoLevel()
{
	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlayLevel()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	///* For. Prototype_Component_Model_Example*/
	//PreTransformMatrix =  XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Example"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Example/Example.fbx","../Bin/ModelData/NonAnim/Example/", PreTransformMatrix))))
	//	return E_FAIL;
	//
	///* For. Prototype_Component_Model_Fiona*/
	//PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", "../Bin/ModelData/Anim/Fiona/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_PlayerExample*/
	//PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PlayerExample"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/PlayerExample/PlayerExample.fbx", "../Bin/ModelData/Anim/PlayerExample/", PreTransformMatrix))))
	//	return E_FAIL;

	// 일반 몬스터 로드 함수
	//if (FAILED(Ready_Resources_For_BasicMonster()))
	//	return E_FAIL;

	// 보스 몬스터 로드 함수
	//if (FAILED(Ready_Resources_For_BossMonster()))
	//	return E_FAIL;

	if (FAILED(Ready_Resources_For_Effect()))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("변환 완료"));

	MSG_BOX(TEXT("변환이 완료되었습니다 종료해주세요"));

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_BossMonster()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

#pragma region Raxasia
	/* For. Prototype_Component_Model_Monster_Raxasia_P1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Raxasia_P1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Boss/Raxasia/Raxasia_P1.fbx", "../Bin/ModelData/Anim/Monster/Boss/Raxasia/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Monster_Raxasia_P2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Raxasia_P2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Boss/Raxasia/Raxasia_P2.fbx", "../Bin/ModelData/Anim/Monster/Boss/Raxasia/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Monster_Raxasia_Shield01 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Raxasia_Shield01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Boss/Raxasia/Raxasia_Shield01.fbx", "../Bin/ModelData/Anim/Monster/Boss/Raxasia/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Monster_Raxasia_Shield02 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Raxasia_Shield02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Boss/Raxasia/Raxasia_Shield02.fbx", "../Bin/ModelData/Anim/Monster/Boss/Raxasia/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Monster_Raxasia_Sword01 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Raxasia_Sword01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Boss/Raxasia/Raxasia_Sword01.fbx", "../Bin/ModelData/Anim/Monster/Boss/Raxasia/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Monster_Raxasia_Sword02 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Raxasia_Sword02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Boss/Raxasia/Raxasia_Sword02.fbx", "../Bin/ModelData/Anim/Monster/Boss/Raxasia/", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion

#pragma region SimonManus
	/* For. Prototype_Component_Model_Monster_SimonManus_P1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_SimonManus_P1"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Boss/SimonManus/SimonManus_P1.fbx", "../Bin/ModelData/Anim/Monster/Boss/SimonManus/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Monster_SimonManus_P2 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_SimonManus_P2"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Boss/SimonManus/SimonManus_P2.fbx", "../Bin/ModelData/Anim/Monster/Boss/SimonManus/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Monster_SimonMaus_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_SimonMaus_Weapon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Boss/SimonManus/SimonMaus_Weapon.fbx", "../Bin/ModelData/Anim/Monster/Boss/SimonManus/", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_BasicMonster()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For. Prototype_Component_Model_Monster_CracassNormal */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_CracassNormal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/CarcassNormal/CarcassNormal.fbx", "../Bin/ModelData/Anim/Monster/CarcassNormal/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Monster_CracassTail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_CracassTail"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/CarcassTail/CarcassTail.fbx", "../Bin/ModelData/Anim/Monster/CarcassTail/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Monster_CarcassBigA */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_CarcassBigA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/CarcassBigA/CarcassBigA.fbx", "../Bin/ModelData/Anim/Monster/CarcassBigA/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Monster_Horesman */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Horesman"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Horesman/Horesman.fbx", "../Bin/ModelData/Anim/Monster/Horesman/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Monster_MissileTurret */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_MissileTurret"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/MissileTurret/MissileTurret.fbx", "../Bin/ModelData/Anim/Monster/MissileTurret/", PreTransformMatrix))))
		return E_FAIL;

	//여기서부터 무기 있는 놈
#pragma region RebornerBigA
	/* For. Prototype_Component_Model_Monster_RebornerBigA */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_RebornerBigA"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/RebornerBigA/RebornerBigA.fbx", "../Bin/ModelData/Anim/Monster/RebornerBigA/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Monster_RebornerBigA_Hat */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_RebornerBigA_Hat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/RebornerBigA/RebornerBigA_Hat.fbx", "../Bin/ModelData/Anim/Monster/RebornerBigA/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Monster_RebornerBigA_Tank */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_RebornerBigA_Tank"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/RebornerBigA/RebornerBigA_Tank.fbx", "../Bin/ModelData/Anim/Monster/RebornerBigA/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Monster_RebornerBigA_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_RebornerBigA_Weapon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/RebornerBigA/RebornerBigA_Weapon.fbx", "../Bin/ModelData/Anim/Monster/RebornerBigA/", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion


#pragma region RebornerMale
	/* For. Prototype_Component_Model_Monster_RebornerMale */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_RebornerMale"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/RebornerMale/RebornerMale.fbx", "../Bin/ModelData/Anim/Monster/RebornerMale/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Monster_RebornerMale_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_RebornerMale_Weapon"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/RebornerMale/RebornerMale_Weapon.fbx", "../Bin/ModelData/Anim/Monster/RebornerMale/", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Effect()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	/* For. Prototype_Component_Effect_Test */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Effect_Test"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/Test.fbx", "../Bin/ModelData/NonAnim/Effect/", PreTransformMatrix))))
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

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
