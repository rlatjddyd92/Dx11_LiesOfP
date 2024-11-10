#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

#include "UIPage_Defines.h"
#include "UIRender_Client.h"
#include "UI_Enum.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUIManager : public CUIObject
{
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

private:
	HRESULT Load_UIDataFile();
	HRESULT Make_UIPage(_int iIndex);
	HRESULT Load_UIDataFile_Part(HANDLE handle, DWORD* dword, _int iIndex);


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
	//// 메인
	//CUIPage_Main* m_pUIPage_Main = { nullptr };
	//// 메인
	//CUIPage_Main* m_pUIPage_Main = { nullptr };
	//// 메인
	//CUIPage_Main* m_pUIPage_Main = { nullptr };
	//// 메인
	//CUIPage_Main* m_pUIPage_Main = { nullptr };
	//// 메인
	//CUIPage_Main* m_pUIPage_Main = { nullptr };




	CUIRender_Client* m_pUIRender_Client = { nullptr };

	// 플레이 모드 진입 
	_bool m_bPlayMode = false;

	

public:
	static CUIManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END