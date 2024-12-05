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

	/* For. Prototype_Component_Model_PlayerExample*/
	//PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Player/Player.fbx", "../Bin/ModelData/Anim/Player/", PreTransformMatrix))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monastery_TowerDoor"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monastery_TowerDoor/SK_FO_Monastery_TowerDoor_01.fbx", "../Bin/ModelData/NonAnim/InteractObj/Tex/", PreTransformMatrix))))
	//	return E_FAIL;


	///* For. Prototype_Component_Model_Stargazer*/
	//PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Stargazer"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Stargazer/SK_DLV_Stargazer_01.fbx", "../Bin/ModelData/NonAnim/InteractObj/Tex/", PreTransformMatrix))))
	//	return E_FAIL;
	
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monastery_TheLastDoor"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monastery_TheLastDoor/SK_FO_Monastery_TheLastDoor_01.fbx", "../Bin/ModelData/NonAnim/InteractObj/Tex/", PreTransformMatrix))))
	//	return E_FAIL;

	// 일반 몬스터 로드 함수
	//if (FAILED(Ready_Resources_For_BasicMonster()))
	//	return E_FAIL;
	// 
	//if (FAILED(Ready_Resources_For_BossMonster()))
	//	return E_FAIL;

	// 준호형 전용
	// if (FAILED(Ready_Resources_GoJunHo()))

	if (FAILED(Ready_Resources_For_Effect()))
		return E_FAIL;
	 

	//우송
	 //if (FAILED(Ready_Resources_Object()))
		//return E_FAIL;

	//if (FAILED(Ready_Resources_For_Weapon()))
	//	return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("변환 완료"));

	MSG_BOX(TEXT("변환이 완료되었습니다 종료해주세요"));

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_BossMonster()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

//#pragma region Raxasia
//	/* For. Prototype_Component_Model_Monster_Raxasia_P1 */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Raxasia_P1"),
//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Boss/Raxasia/Raxasia_P1.fbx", "../Bin/ModelData/Anim/Monster/Boss/Raxasia/", PreTransformMatrix))))
//		return E_FAIL;
//
//	/* For. Prototype_Component_Model_Monster_Raxasia_P2 */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Raxasia_P2"),
//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Boss/Raxasia/Raxasia_P2.fbx", "../Bin/ModelData/Anim/Monster/Boss/Raxasia/", PreTransformMatrix))))
//		return E_FAIL;
//
//	/* For. Prototype_Component_Model_Monster_Raxasia_Shield01 */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Raxasia_Shield01"),
//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Boss/Raxasia/Raxasia_Shield01.fbx", "../Bin/ModelData/Anim/Monster/Boss/Raxasia/", PreTransformMatrix))))
//		return E_FAIL;
//
//	/* For. Prototype_Component_Model_Monster_Raxasia_Shield02 */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Raxasia_Shield02"),
//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Boss/Raxasia/Raxasia_Shield02.fbx", "../Bin/ModelData/Anim/Monster/Boss/Raxasia/", PreTransformMatrix))))
//		return E_FAIL;
//
//	/* For. Prototype_Component_Model_Monster_Raxasia_Sword01 */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Raxasia_Sword01"),
//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Boss/Raxasia/Raxasia_Sword01.fbx", "../Bin/ModelData/Anim/Monster/Boss/Raxasia/", PreTransformMatrix))))
//		return E_FAIL;
//
//	/* For. Prototype_Component_Model_Monster_Raxasia_Sword02 */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Raxasia_Sword02"),
//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/Boss/Raxasia/Raxasia_Sword02.fbx", "../Bin/ModelData/Anim/Monster/Boss/Raxasia/", PreTransformMatrix))))
//		return E_FAIL;
//#pragma endregion

#pragma region SimonManus
	///* For. Prototype_Component_Model_Monster_SimonManus_P1 */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_SimonManus_P1"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Boss/SimonManus/SimonManus_P1.fbx", "../Bin/ModelData/Anim/Monster/Boss/SimonManus/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Monster_SimonManus_P2 */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_SimonManus_P2"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Boss/SimonManus/SimonManus_P2.fbx", "../Bin/ModelData/Anim/Monster/Boss/SimonManus/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Monster_SimonMaus_Weapon */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_SimonMaus_Weapon"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Boss/SimonManus/SimonMaus_Weapon.fbx", "../Bin/ModelData/Anim/Monster/Boss/SimonManus/", PreTransformMatrix))))
	//	return E_FAIL;

	/* For. Prototype_Component_Model_Monster_SimonMaus_Hand */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_SimonMaus_Hand"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Boss/SimonManus/SimonManus_Hand.fbx", "../Bin/ModelData/Anim/Monster/Boss/SimonManus/", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_BasicMonster()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	///* For. Prototype_Component_Model_Monster_CracassNormal */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_CracassNormal"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/CarcassNormal/CarcassNormal.fbx", "../Bin/ModelData/Anim/Monster/CarcassNormal/", PreTransformMatrix))))
	//	return E_FAIL;

	/* For. Prototype_Component_Model_Monster_CracassTail */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_CracassTail"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/CarcassTail/CarcassTail.fbx", "../Bin/ModelData/Anim/Monster/CarcassTail/", PreTransformMatrix))))
		return E_FAIL;

//	/* For. Prototype_Component_Model_Monster_CarcassBigA */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_CarcassBigA"),
//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/CarcassBigA/CarcassBigA.fbx", "../Bin/ModelData/Anim/Monster/CarcassBigA/", PreTransformMatrix))))
//		return E_FAIL;
//
//	/* For. Prototype_Component_Model_Monster_Horesman */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_Horesman"),
//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/Horesman/Horesman.fbx", "../Bin/ModelData/Anim/Monster/Horesman/", PreTransformMatrix))))
//		return E_FAIL;
//
//	/* For. Prototype_Component_Model_Monster_MissileTurret */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_MissileTurret"),
//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/MissileTurret/MissileTurret.fbx", "../Bin/ModelData/Anim/Monster/MissileTurret/", PreTransformMatrix))))
//		return E_FAIL;
//
//	//여기서부터 무기 있는 놈
//#pragma region RebornerBigA
//	/* For. Prototype_Component_Model_Monster_RebornerBigA */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_RebornerBigA"),
//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/RebornerBigA/RebornerBigA.fbx", "../Bin/ModelData/Anim/Monster/RebornerBigA/", PreTransformMatrix))))
//		return E_FAIL;
//
//	/* For. Prototype_Component_Model_Monster_RebornerBigA_Hat */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_RebornerBigA_Hat"),
//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/RebornerBigA/RebornerBigA_Hat.fbx", "../Bin/ModelData/Anim/Monster/RebornerBigA/", PreTransformMatrix))))
//		return E_FAIL;
//
//	/* For. Prototype_Component_Model_Monster_RebornerBigA_Tank */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_RebornerBigA_Tank"),
//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/RebornerBigA/RebornerBigA_Tank.fbx", "../Bin/ModelData/Anim/Monster/RebornerBigA/", PreTransformMatrix))))
//		return E_FAIL;
//
//	/* For. Prototype_Component_Model_Monster_RebornerBigA_Weapon */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_RebornerBigA_Weapon"),
//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/RebornerBigA/RebornerBigA_Weapon.fbx", "../Bin/ModelData/Anim/Monster/RebornerBigA/", PreTransformMatrix))))
//		return E_FAIL;
//#pragma endregion
//
//
//#pragma region RebornerMale
//	/* For. Prototype_Component_Model_Monster_RebornerMale */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_RebornerMale"),
//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Monster/RebornerMale/RebornerMale.fbx", "../Bin/ModelData/Anim/Monster/RebornerMale/", PreTransformMatrix))))
//		return E_FAIL;
//
//	/* For. Prototype_Component_Model_Monster_RebornerMale_Weapon */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monster_RebornerMale_Weapon"),
//		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Monster/RebornerMale/RebornerMale_Weapon.fbx", "../Bin/ModelData/Anim/Monster/RebornerMale/", PreTransformMatrix))))
//		return E_FAIL;
//#pragma endregion

	return S_OK;
}

HRESULT CLoader::Ready_Resources_GoJunHo()
{
	/* For. Prototype_Component_Model_Effect_HalfSphere01 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_HalfSphere01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/HalfSphere/SM_HalfSphere_01_GDH.fbx", "../Bin/ModelData/NonAnim/Effect/"))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Effect_HalfSphere02 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Effect_HalfSphere02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/HalfSphere/SM_HalfSphere_02_KMH.fbx", "../Bin/ModelData/NonAnim/Effect/"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_Object()
{
	_matrix		PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monastery_SM_CH_PROP_BrokeDollTrap_01_Doll"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/SM_CH_PROP_BrokeDollTrap_01_Doll.fbx", "../Bin/ModelData/NonAnim/InteractObj/Tex/", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monastery_SM_CH_PROP_BrokeDollTrap_01_Trap"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/SM_CH_PROP_BrokeDollTrap_01_Trap.fbx", "../Bin/ModelData/NonAnim/InteractObj/Tex/", PreTransformMatrix))))
		return E_FAIL;

	////////////////////////

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monastery_SK_DLV_Global_LiftController_01_Normal"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/InteractObj/SK_DLV_Global_LiftController_01_Normal.fbx", "../Bin/ModelData/NonAnim/InteractObj/Tex/", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monastery_SK_FO_LadderActivater"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/InteractObj/SK_FO_LadderActivater.fbx", "../Bin/ModelData/NonAnim/InteractObj/Tex/", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monastery_SK_FO_Monastery_Lift_01_Controller"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/InteractObj/SK_FO_Monastery_Lift_01_Controller.fbx", "../Bin/ModelData/NonAnim/InteractObj/Tex/", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monastery_SK_FO_Monastery_TowerDoor_01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/InteractObj/SK_FO_Monastery_TowerDoor_01.fbx", "../Bin/ModelData/NonAnim/InteractObj/Tex/", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monastery_SK_FO_Monastery_WallDeco_01_Scupture04"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/InteractObj/SK_FO_Monastery_WallDeco_01_Scupture04.fbx", "../Bin/ModelData/NonAnim/InteractObj/Tex/", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monastery_SK_FO_TreasureChest_02_Red"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/InteractObj/SK_FO_TreasureChest_02_Red.fbx", "../Bin/ModelData/NonAnim/InteractObj/Tex/", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monastery_SK_LV_Ladder_MetalWood_01_KSJ"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/InteractObj/SK_LV_Ladder_MetalWood_01_KSJ.fbx", "../Bin/ModelData/NonAnim/InteractObj/Tex/", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monastery_SK_LV_Ladder_MetalWood_Slide6m_SM_KSJ"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/InteractObj/SK_LV_Ladder_MetalWood_Slide6m_SM_KSJ.fbx", "../Bin/ModelData/NonAnim/InteractObj/Tex/", PreTransformMatrix))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Monastery_SK_NewTown_Lift_01_Door"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/InteractObj/SK_NewTown_Lift_01_Door.fbx", "../Bin/ModelData/NonAnim/InteractObj/Tex/", PreTransformMatrix))))
		return E_FAIL;	

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SK_DLV_Stargazer_Broken_01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/InteractObj/SK_DLV_Stargazer_Broken_01.fbx", "../Bin/ModelData/NonAnim/InteractObj/Tex/", PreTransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Effect()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	_wstring searchPath = TEXT("../Bin/Resources/Models/Effect/*.fbx");
	WIN32_FIND_DATAW findFileData;
	HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		MSG_BOX(TEXT("Folder Open Failed"));
		return E_FAIL;
	}

	while (FindNextFileW(hFind, &findFileData))
	{
		if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			_wstring strFileName = findFileData.cFileName;   // 파일 이름 + 확장자

			_wstring strFileTag = strFileName;
			size_t dotPos = strFileTag.find_last_of(L'.');
			if (dotPos == _wstring::npos) {
				strFileTag = TEXT(""); // 확장자가 없으면 빈 문자열 반환
			}

			_wstring strExtention = strFileTag.substr(dotPos + 1);

			size_t nullPos = strExtention.find(L'\0');
			if (nullPos != _wstring::npos) {
				strExtention.erase(nullPos);
			}

			_wstring strFileExtention = strExtention;   // 확장자

			strFileTag = strFileName;
			dotPos = strFileTag.rfind(L'.');
			if (dotPos == _wstring::npos) {
				// 확장자가 없으면 원본 반환
				strFileTag = TEXT(""); // 확장자가 없으면 빈 문자열 반환
			}
			_wstring strFileBaseName = strFileTag.substr(0, dotPos);

			_wstring strResultPath = TEXT("../Bin/Resources/Models/Effect/") + strFileName;

			_wstring strPrototypeTag = TEXT("Prototype_Component_Model_Effect_") + strFileBaseName;


			strResultPath;

			// 변환 후 필요한 버퍼 크기 계산
			int bufferSize = WideCharToMultiByte(CP_UTF8, 0, strResultPath.c_str(), -1, nullptr, 0, nullptr, nullptr);
			if (bufferSize == 0) {
				throw std::runtime_error("WideCharToMultiByte failed.");
			}

			// 변환 결과를 저장할 std::string 생성
			std::string str(bufferSize, '\0');
			WideCharToMultiByte(CP_UTF8, 0, strResultPath.c_str(), -1, &str[0], bufferSize, nullptr, nullptr);

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strPrototypeTag,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, str.c_str(), "../Bin/ModelData/NonAnim/Effect/", PreTransformMatrix))))
			{
				int a = 0;
				return E_FAIL;
			}
		}
	}
	// 핸들 닫기
	FindClose(hFind);


	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_Weapon()
{
	_matrix		PreTransformMatrix = XMMatrixIdentity();

	///* For. Prototype_Component_Model_Weapon_FlameSword */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_FlameSword"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapon/FlameSword.fbx", "../Bin/ModelData/NonAnim/Weapon/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Weapon_Rapier */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Rapier"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapon/Rapier.fbx", "../Bin/ModelData/NonAnim/Weapon/", PreTransformMatrix))))
	//	return E_FAIL;

	///* For. Prototype_Component_Model_Weapon_Scissor_Combine */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Scissor_Combine"),
	//	CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapon/Scissor_Combine.fbx", "../Bin/ModelData/NonAnim/Weapon/", PreTransformMatrix))))
	//	return E_FAIL;

	/* For. Prototype_Component_Model_Weapon_Scissor_Left_Bld */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Scissor_Left_Bld"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapon/Scissor_Left_Blade.fbx", "../Bin/ModelData/NonAnim/Weapon/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Weapon_Scissor_Left_Hnd */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Scissor_Left_Hnd"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapon/Scissor_Left_Handle.fbx", "../Bin/ModelData/NonAnim/Weapon/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Weapon_Scissor_Right_Bld */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Scissor_Right_Bld"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapon/Scissor_Right_Blade.fbx", "../Bin/ModelData/NonAnim/Weapon/", PreTransformMatrix))))
		return E_FAIL;

	/* For. Prototype_Component_Model_Weapon_Scissor_Right_Hnd */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Weapon_Scissor_Right_Hnd"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapon/Scissor_Right_Handle.fbx", "../Bin/ModelData/NonAnim/Weapon/", PreTransformMatrix))))
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
