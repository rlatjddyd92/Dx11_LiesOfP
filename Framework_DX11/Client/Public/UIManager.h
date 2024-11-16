#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

#include "UIPage_Defines.h"
#include "UIRender_Client.h"
#include "Interface_Enums.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUIManager : public CUIObject
{
public:
	typedef struct TESTDATA
	{
		// 직교 UI 테스트용 
		_float fHP_Now = 500.f;
		_float fHP_Max = 1000.f;

		_float fHP_Damege = 100.f;

		_bool bFocus = false;
		_bool bSpecial_Attack = false;

	}TDATA;




private:
	CUIManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIManager(const CUIManager& Prototype);
	virtual ~CUIManager() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Update_UIManager(_float fTimeDelta);
	void Update_UIControl(_float fTimeDelta);

private:
	void UIControl_Test(_float fTimeDelta);
	void UIControl_Main(_float fTimeDelta);
	void UIControl_Loading(_float fTimeDelta);
	void UIControl_Play(_float fTimeDelta);
	void UIControl_Inven(_float fTimeDelta);

public:
#pragma region Page_Main
	void OpenMainPage();
#pragma endregion

#pragma region Page_Loading
	void OpenLoadingPage();
#pragma endregion

#pragma region PlayMode
	// 플레이 모드 진입
	_bool GetPlayMode() { return m_bPlayMode; }
	void SetPlayMode(_bool bIsPlayMode)
	{
		m_vecPage[_int(UIPAGE::PAGE_LOADING)]->SetRender(false);
		m_bPlayMode = bIsPlayMode;
	}

	// 화면 전환 
	void SwicthPage(UIPAGE eNextPage);



#pragma endregion


#pragma region Page_Ortho
	void Register_Pointer_Into_OrthoUIPage(UI_ORTHO_OBJ_TYPE eType, void* pObj) { m_pUIPage_Ortho->Register_Pointer_Into_OrthoUIPage(eType, pObj); }


#pragma endregion

	TDATA* GetTestData() { return m_pTestData; } // 테스트용 조작을 위한 모든 것을 넣어 두는 함수
	// 추후 신속한 정리를 위해 테스트용 조작은 모두 여기에 넣고 추후 한번에 제거하기 
	// 위 함수와 관련된 것은 테스트용 임시 내용을 다루므로 편의성을 중시한 구조로 만들 것임 따라서 정식 내용은 넣으면 안됨 

	// TestPage 
	void ShowTestPage(TEST_PAGE_NAME eName,
		_wstring DataNameA, TEST_PAGE_VALUE_TYPE eTypeA, const void* ValueA,
		_wstring DataNameB = {}, TEST_PAGE_VALUE_TYPE eTypeB = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueB = nullptr,
		_wstring DataNameC = {}, TEST_PAGE_VALUE_TYPE eTypeC = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueC = nullptr,
		_wstring DataNameD = {}, TEST_PAGE_VALUE_TYPE eTypeD = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueD = nullptr,
		_wstring DataNameE = {}, TEST_PAGE_VALUE_TYPE eTypeE = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueE = nullptr,
		_wstring DataNameF = {}, TEST_PAGE_VALUE_TYPE eTypeF = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueF = nullptr,
		_wstring DataNameG = {}, TEST_PAGE_VALUE_TYPE eTypeG = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueG = nullptr,
		_wstring DataNameH = {}, TEST_PAGE_VALUE_TYPE eTypeH = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueH = nullptr,
		_wstring DataNameI = {}, TEST_PAGE_VALUE_TYPE eTypeI = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueI = nullptr,
		_wstring DataNameJ = {}, TEST_PAGE_VALUE_TYPE eTypeJ = TEST_PAGE_VALUE_TYPE::TYPE_END, const void* ValueJ = nullptr
	); // 함수 사용하면 입력 내용 대로 해당 프레임에 창을 띄워 준다, DataNameA: 데이터의 이름, TEST_PAGE_VALUE_TYPE: 자료형(float,int,tchar), ValueA: 값을 받아올 포인터 <- 이렇게 한줄
	// 매 프레임 마다 함수를 사용해줘야 작동함 (Gameinterface가 외부 정보를 저장/관리하지 않도록 하기 위함)




private:
	HRESULT Load_UIDataFile();
	HRESULT Make_UIPage(_int iIndex);
	HRESULT Load_UIDataFile_Part(HANDLE handle, DWORD* dword, _int iIndex);

	void Setting_TestPage();

	void InputTestPageInfo(vector<_wstring>* pName, vector<_wstring>* pValue, _wstring DataNameA, TEST_PAGE_VALUE_TYPE eTypeA, const void* ValueA);

private:
	vector<CUIPage*> m_vecPage; // <- 렌더러 참조용

	// 페이지 별 포인터
	// 캐스팅을 하지 않기 위해 추가
	// AddRef하지 않고 사용

	// 메인
	CUIPage_Main* m_pUIPage_Main = { nullptr };
	// 로딩
	CUIPage_Loading* m_pUIPage_Loading = { nullptr };
	// 플레이
	CUIPage_Play* m_pUIPage_Play = { nullptr };
	// 메뉴
	CUIPage_Menu* m_pUIPage_Menu = { nullptr };
	// 인벤
	CUIPage_Inven* m_pUIPage_Inven = { nullptr };
	// 장비
	CUIPage_Equip* m_pUIPage_Equip = { nullptr };
	// 스탯
	CUIPage_Stat* m_pUIPage_Stat = { nullptr };
	// 레벨업
	CUIPage_LevelUp* m_pUIPage_LevelUp = { nullptr };
	// 스킬트리
	CUIPage_Skill* m_pUIPage_Skill = { nullptr };
	// 테스트
	CUIPage_Test* m_pUIPage_Test = { nullptr };
	// 직교
	CUIPage_Ortho* m_pUIPage_Ortho = { nullptr };



	CUIRender_Client* m_pUIRender_Client = { nullptr };

	// 플레이 모드 진입 
	_bool m_bPlayMode = false;

	// test code
#ifdef _DEBUG
	_int m_iFonttest = 0; // -1 : 영어, 0 : off, 1 : 한글
#endif

#pragma region TestData
	TESTDATA* m_pTestData = { nullptr }; // UI 및 기타 테스트를 위한 임시 데이터 구조체 
	// GameInterface 싱글톤으로 아무나 접근 및 수정이 가능함 
	// 정식 데이터는 이렇게 접근/수정 불가능하도록 만들 예정

	vector<_float2> m_vecTestPage_Pos;
	vector<_float2> m_vecTestPage_Size;
	vector<_float2> m_vecTestPage_ClickPos;

	vector<_bool> m_vecTestPageOpen;
	vector<_bool> m_vecTestPageMove;




#pragma endregion

public:
	static CUIManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END