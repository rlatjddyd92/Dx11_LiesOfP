#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Option : public CUIPage
{
public:
	enum class PART_GROUP
	{
		OPTION_Back,
		OPTION_Back_Grid,
		OPTION_Back_Fx,

		OPTION_Header_Frame,
		OPTION_Tap_Q_Button,
		OPTION_Tap_Mouse_Area,
		OPTION_Tap_Text_Fx,
		OPTION_Tap_Text,
		OPTION_Tap_Highlight_Line_Pos,
		OPTION_Tap_E_Button,
		
		OPTION_Main_Scroll_Area,
		OPTION_Main_Scroll_Bar_Line,
		OPTION_Main_Scroll_Bar,
		
		OPTION_TITLE_Area,
		OPTION_TITLE_Text,
		OPTION_TITLE_Underline,
		
		OPTION_LINE_Area,
		OPTION_LINE_Focus_Pos,
		OPTION_LINE_Text,
		OPTION_LINE_Underline,
		
		OPTION_FUNC_Dropbox_Area,
		OPTION_FUNC_Dropbox_Selected_Text,
		OPTION_FUNC_Dropbox_Arrow,
		OPTION_FUNC_Dropbox_Box_Header,
		OPTION_FUNC_Dropbox_Box_Area,
		OPTION_FUNC_Dropbox_Select_Area,
		OPTION_FUNC_Dropbox_Select_Focus_Pos,
		OPTION_FUNC_Dropbox_Select_Text,
		OPTION_FUNC_Dropbox_Scroll_Line,
		OPTION_FUNC_Dropbox_Scroll_Bar,
		
		OPTION_FUNC_Bool_Frame,
		OPTION_FUNC_Bool_Left_Area,
		OPTION_FUNC_Bool_Left_Text,
		OPTION_FUNC_Bool_Left_Focus_Pos,
		OPTION_FUNC_Bool_Left_UnderLine,
		OPTION_FUNC_Bool_Right_Area,
		OPTION_FUNC_Bool_Right_Text,
		OPTION_FUNC_Bool_Right_Focus_Pos,
		OPTION_FUNC_Bool_Right_UnderLine,
		
		OPTION_FUNC_Slide_Area,
		OPTION_FUNC_Slide_Num,
		OPTION_FUNC_Slide_Bar_Line,
		OPTION_FUNC_Slide_Left_Arrow,
		OPTION_FUNC_Slide_Bar_Button,
		OPTION_FUNC_Slide_Right_Arrow,
		
		OPTION_DESC_Head,
		OPTION_DESC_Deco,
		OPTION_DESC_Text,
		
		OPTION_Focus,
		OPTION_Highlight_Line,
		
		OPTION_Calibration_Frame,
		OPTION_Calibration_Image,
		OPTION_Calibration_Text,





		GROUP_END
	};

	


	// 구조체 
	// Tab -> Line -> Funtion -> (Dropbox의 경우) DropboxInfo

	typedef struct OPTION_FUNCTION
	{
		OPTION_FUNCTION() {}
		~OPTION_FUNCTION() { vecSelect_Name.clear(); }
		// bool 
		_bool bIsSelect_Left = true;

		_wstring strLeft = {};
		_wstring strRight = {};

		// dropbox 
		_bool bDropBox_On = false;
		_int iSize_Select_Button = 0;
		_int iSelected_Num = 0;
		vector<_wstring> vecSelect_Name;
		
		// slide 
		_bool bIsShow_IntNum = false;
		_bool bIsButton_Moving = false;
		_float fMin = 0.f;
		_float fNow = 0.f;
		_float fMax = 0.f;
		_float fGap = 0.f;
		_float fInterval = 0.f;
		_float fInterval_X = 0.f;
		_float fInterval_Ratio = 0.f;
		_float fNow_Ratio = 0.f;

		_wstring strVariable_Key = {};

	}FUNCTION;

	typedef struct OPTION_LINE
	{
		OPTION_LINE() {}
		~OPTION_LINE() { Safe_Delete(pFunction); }

		OPTION_FUNC eFunc = OPTION_FUNC::FUNC_END;
		_wstring strName = {};
		FUNCTION* pFunction = { nullptr };
	}LINE;

	typedef struct OPTION_TAB
	{
		OPTION_TAB() {}
		~OPTION_TAB() 
		{
			for (auto& iter : vecOption_Line)
				Safe_Delete(iter);
			
			vecOption_Line.clear();
		}
		_wstring strName = {};
		vector<LINE*> vecOption_Line;
	}TAB;

protected:
	CUIPage_Option(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Option(const CUIPage_Option& Prototype);
	virtual ~CUIPage_Option() = default;

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
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }
	virtual HRESULT Ready_UIPart_Group_Control() override;

public: // UI 매니저 & 싱글톤이 사용하는 함수 
	_bool Get_Variable(_wstring strKey, _float* fVariable) 
	{
		map<_wstring, _float>::iterator iter = m_mapVariables.find(strKey);
		if (iter == m_mapVariables.end())
			return false;
		else
			return (*iter).second;
	}


protected: // 자체 사용 함수 
	HRESULT Initialize_Option();

	void Update_Tab(_float fTimeDelta);
	void Update_Scroll(_float fTimeDelta);
	void Update_Line(_float fTimeDelta);

	void Update_BoolButton(FUNCTION& NowFunction, _float fTimeDelta, _bool bMouse);
	void Update_Slide(FUNCTION& NowFunction, _float fTimeDelta, _bool bMouse);
	void Update_Slide_Button(FUNCTION& NowFunction, _float fTimeDelta, _bool bMouse);
	void Update_Dropbox(FUNCTION& NowFunction, _float fTimeDelta, _bool bMouse);

	void Open_Dropbox_SelectBox(_float fTimeDelta);
	void Close_Dropbox_SelectBox(_float fTimeDelta);
	
	
	void Activate_OptionScroll(_float fTimeDelta);
	void DeActivate_OptionScroll(_float fTimeDelta);

protected:
	SCROLL* m_pScroll_Option = { nullptr };
	SCROLL* m_pScroll_DropBox = { nullptr };

	vector<TAB*> m_vecOption_TabInfo;

	_int m_iNow_Tab = 0;
	_int m_iNow_Line = 0;
	_bool m_bIsOpen_DropBox = false;
	_bool m_bChange_Tab = false;

	map<_wstring, _float> m_mapVariables; 

	_Vec2 m_vOrigin_Focus_Dest = { 0.f,0.f };

public:
	static CUIPage_Option* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END