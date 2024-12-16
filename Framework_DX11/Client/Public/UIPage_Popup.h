#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Popup : public CUIPage
{
public:
	enum class PART_GROUP
	{
		POPUP_Top,
		POPUP_Middle,
		POPUP_Bottom,
		POPUP_Title,
		POPUP_Desc_0,
		POPUP_Desc_1,

		POPUP_Mouse_0,
		POPUP_Space,
		POPUP_Text_0,

		POPUP_Mouse_1,
		POPUP_R,
		POPUP_Text_1,

		POPUP_Input_Count_Back,
		POPUP_Input_Count_Text,
		POPUP_Input_Count_Decrease,
		POPUP_Input_Count_Num,
		POPUP_Input_Count_Increase,

		POPUP_Count_Back,
		POPUP_Count_Text,
		POPUP_Count_Icon,
		POPUP_Count_Num,






		GROUP_END
	};

	typedef struct ITEMPOPUP_INFO
	{
		_bool bIsActive = false;

		_int iMin = 0;
		_int iNow = 0;
		_int iMax = 0;

		_int* pNow_Input = { nullptr };

		_int iInterval = 0;
		_int iNow_Count = 0;
		
		_int* pNow_Count = { nullptr };
	}ITEMPOPUP;


protected:
	CUIPage_Popup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Popup(const CUIPage_Popup& Prototype);
	virtual ~CUIPage_Popup() = default;

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

public: 
	void Show_Popup(_wstring strTitle, _wstring strDescA, _wstring strDescB = TEXT("none"));
	void Off_Popup();

	void Show_ItemPopup(_wstring strTitle, _wstring strInputTitle = TEXT("none"), _int iMin = 0, _int* pNow_Input = nullptr, _int iMax = 0, _wstring strCountTitle = TEXT("none"), _int iInterval = 0, _int* pNow_Count = nullptr);

	void Show_ItemUsePopup(_wstring strTitle, _wstring strDescA, _bool bIsTop);

	void Show_TrueFalsePopup(_wstring strTitle, _wstring strDescA, _int* iResult); // true,false,none 3개 상태 표현 위해 _int 사용

public:
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }
	virtual HRESULT Ready_UIPart_Group_Control() override;

protected:
	ITEMPOPUP_INFO* m_pItemPopup_Info = { nullptr };

	_bool m_bItemUsePopUp = false;
	_bool m_bTopBelt = false;

	_int* m_pTrueFalsePopup_Result = { nullptr };
	

public:
	static CUIPage_Popup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END