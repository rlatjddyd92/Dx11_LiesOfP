#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)
class CUITutorial_Info;
class CUITutorial_Guide;
class CUITutorial_Timing;
class CUITutorial_Result;
class CUITutorial_Popup;

class CUIPage_Tutorial : public CUIPage
{
public:
	enum class PART_GROUP
	{
		GROUP_CENTER,
		GROUP_INFO_FRAME,
		GROUP_INFO_MISSION,
		GROUP_INFO_SCORE,
		GROUP_GUIDE,
		GROUP_TIMING,
		GROUP_RESULT,
		GROUP_POPUP,
		GROUP_END
	};

	typedef struct
	{
		_wstring strName = {};
		_wstring strDescA = {};
		_wstring strDescB = {};

		_int m_iGoal = 0;
		_int m_iScore = 0;

		KEY eKey_A_First = KEY::N;
		KEY eKey_A_Second = KEY::N;
		_wstring strKey_A_Desc = {};
		KEY eKey_B_First = KEY::N;
		KEY eKey_B_Second = KEY::N;
		_wstring strKey_B_Desc = {};
	}TUTO_INFO;


protected:
	CUIPage_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Tutorial(const CUIPage_Tutorial& Prototype);
	virtual ~CUIPage_Tutorial() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OpenAction() override;
	virtual void CloseAction() override;

	virtual CHECK_MOUSE Check_Page_Action(_float fTimeDelta) override;

	void Close_Popup();
	void Open_Popup();
	
	_bool Get_TutorialPopup_Open() { return m_bPopupOpen; }

public:
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }
	virtual HRESULT Ready_UIPart_Group_Control() override;

protected:
	void Initialize_Tutorial();
	void Update_Tutorial();

	// UI °´Ã¼ Á¦¾î 
	void Update_Tutorial_Info(_float fTimeDelta);
	void Update_Tutorial_Guide(_float fTimeDelta);
	void Update_Tutorial_Timing(_float fTimeDelta);
	void Update_Tutorial_Result(_float fTimeDelta);
	void Update_Tutorial_Popup(_float fTimeDelta);

	void ShowTiming(KEY eKey, _float fTime);


protected:
	vector<class CUITutorial_Info*> m_vecInfo = { nullptr };
	class CUITutorial_Guide* m_pGuide = { nullptr };
	class CUITutorial_Timing* m_pTiming = { nullptr };
	class CUITutorial_Result* m_pResult = { nullptr };
	class CUITutorial_Popup* m_pPopup = { nullptr };

	vector<TUTO_INFO*> m_vecTutorial_Data;
	_bool m_bPopupOpen = false;

public:
	static CUIPage_Tutorial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END