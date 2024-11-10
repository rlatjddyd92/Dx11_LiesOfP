#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "GameInstance.h"
#include "UIManager.h"


BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CGameInterface_Controller : public CBase
{
	DECLARE_SINGLETON(CGameInterface_Controller)

public:
	typedef struct TESTDATA
	{
		_float fHP_Now = 500.f; // 현재 체력값 
		_float fMax_HP_Now = 1000.f; // 체력 최대치의 현재값
		_float fMax_HP_Limit = 1000.f; // 체력 최대치의 최대 증가값

		_float fST_Now = 500.f; // 현재 스태미나값 
		_float fMax_ST_Now = 1000.f; // 스태미나 최대치의 현재값
		_float fMax_ST_Limit = 1000.f; // 스태미나 최대치의 최대 증가값
		
		_float fSP_Now = 100.f; // 현재 특수능력치
		_float fSP_Single_Max = 100.f; // 특수능력치 게이지 한 칸의 최대치 
		_int iSP_Gauge_Count = 5; // 현재 특수능력치 게이지 칸 
		_int iSP_Gauge_Count_Max = 5; // 특수능력치 게이지 최대치 

	}TDATA;


private:
	CGameInterface_Controller();
	virtual ~CGameInterface_Controller() = default;






public: // 외부에서 매니저 접근 

#pragma region TestData
	TESTDATA* m_pTestData = { nullptr }; // UI 및 기타 테스트를 위한 임시 데이터 구조체 
	// GameInterface 싱글톤으로 아무나 접근 및 수정이 가능함 
	// 정식 데이터는 이렇게 접근/수정 불가능하도록 만들 예정
#pragma endregion


#pragma region Common
	HRESULT Initialize_GameInterface(ID3D11Device** pDevice, ID3D11DeviceContext** pContext, CGameInstance* pInstance);
	void Update_GameInterface(_float fTimeDelta);
	void Release_GameInterface();
	TDATA* GetTestData() {return m_pTestData;} // 테스트용 조작을 위한 모든 것을 넣어 두는 함수
	// 추후 신속한 정리를 위해 테스트용 조작은 모두 여기에 넣고 추후 한번에 제거하기 
	// 위 함수와 관련된 것은 테스트용 임시 내용을 다루므로 편의성을 중시한 구조로 만들 것임 따라서 정식 내용은 넣으면 안됨 

#pragma endregion


#pragma region UIManager
	void OpenMainPage() { m_pUIManager->OpenMainPage(); }
	void OpenLoadingPage() { m_pUIManager->OpenLoadingPage(); }

	_bool GetPlayMode() { return m_pUIManager->GetPlayMode(); }
	void SetPlayMode(_bool bIsPlayMode) { m_pUIManager->SetPlayMode(bIsPlayMode); }

	void SwicthPage(UIPAGE eNextPage) { m_pUIManager->SwicthPage(eNextPage); }


#pragma endregion


#pragma region ItemManager



#pragma endregion




private:
	CGameInstance* m_pGameInstance = { nullptr };
	CUIManager* m_pUIManager = { nullptr };





private: // <- 디바이스 
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END
