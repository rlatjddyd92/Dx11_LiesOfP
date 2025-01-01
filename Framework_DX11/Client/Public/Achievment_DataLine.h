#pragma once

#include "Client_Defines.h"
#include "UIPage_Achievment.h"

BEGIN(Engine)

END

BEGIN(Client)

class CAchievment_DataLine : public CUIPage_Achievment
{
protected:
	CAchievment_DataLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAchievment_DataLine(const CAchievment_DataLine& Prototype);
	virtual ~CAchievment_DataLine() = default;

private:
	

public:
	// 업적 데이터 관리
	HRESULT Initialize_Data(vector<struct CUIPage::UIPART_INFO*>& vecOrigin);
	void Input_Data(vector<_wstring>& Data);

	void Add_Stat(_int iStat)
	{
		if (m_bIsComplete == false)
		{
			m_iNow = min(m_iNow + iStat, m_iGoal);
			m_iNow = max(m_iNow, 0);

			if (m_iNow >= m_iGoal)
			{
				m_bIsComplete = true;
				m_iNow = m_iGoal;
				m_vLifeTime.x = 0.01f;
			}
		}
	}

	void Set_Scroll(struct CUIPage::SCROLL_INFO* Scroll) { m_pScroll = Scroll; }

	// 업적 콘트롤 
	_bool Update_Line(_float fTimeDelta, _float fAdjustY, _float fAlpha, _bool bIsRender);

	// 팝업 컨트롤
	_bool Is_Complete() { return m_bIsComplete; }
	void Update_PopUp(_float fTimeDelta, _float fPopupHeight);
	_bool Is_PopupOn() { return m_vLifeTime.x > 0.f; }
	
private:
	void Set_Before_LineRender(_float fAdjustY, _float fAlpha);
	void Set_Before_PopUpRender(_float fTimeDelta, _float fPopupHeight);
	void Set_LinePart();
	void Set_PopupPart();
	void Render_Line(_float fAlpha);
	_bool Check_AllowPopup();
	void Determine_PopupAlpha(_float fTimeDelta);
	void Render_Popup();

	void Set_PartArray(vector<struct CUIPage::UIPART_INFO*>* pArray, _Vec2& vPosFrame, _int iStart = 0);
	void Render_PartArray(vector<struct CUIPage::UIPART_INFO*>* pArray, _bool bIsPopup);

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

private:
	//아래 내용은 얕은 복사 -> Release 금지
	vector<struct CUIPage::UIPART_INFO*> m_vecBackCtrl;
	vector<struct CUIPage::UIPART_INFO*> m_vecIconCtrl;
	vector<struct CUIPage::UIPART_INFO*> m_vecTitleCtrl;
	vector<struct CUIPage::UIPART_INFO*> m_vecDescCtrl;
	vector<struct CUIPage::UIPART_INFO*> m_vecCountCtrl;
	vector<struct CUIPage::UIPART_INFO*> m_vecResultCtrl;

	vector<struct CUIPage::UIPART_INFO*> m_vecPopup_BackCtrl;
	vector<struct CUIPage::UIPART_INFO*> m_vecPopup_IconCtrl;
	vector<struct CUIPage::UIPART_INFO*> m_vecPopup_TitleCtrl;
	vector<struct CUIPage::UIPART_INFO*> m_vecPopup_DescCtrl;

private:
	_float m_fPosY_By_Index = 0;
	_int m_iLineIndex[2] = { 0,0 };
	_int m_iPopupIndex[2] = { 0,0 };

	// 업적 데이터 
	_int m_iAchievment_Index = -1;
	_int m_iICon_Index = -1;
	_wstring m_strTitle = {};
	_wstring m_strDesc = {};
	_int m_iNow = 0;
	_int m_iGoal = 0;
	_bool m_bIsComplete = false;
	_float m_fAlpha = 0;

	// 팝업 데이터 
	_float m_fPopup_AlphaNow = 0.f;
	_Vec3 m_vLifeTime = {0.f,1.f,4.f};

	// 렌더 데이터 
	_Vec4 m_vBackColor_Origin = { 0.f,0.f,0.f,0.f };
	_Vec4 m_vIconColor_Origin = { 0.f,0.f,0.f,0.f };
	_Vec4 m_vTitleColor_Origin = { 0.f,0.f,0.f,0.f };
	_Vec4 m_vDescColor_Origin = { 0.f,0.f,0.f,0.f };

	struct CUIPage::SCROLL_INFO* m_pScroll = { nullptr };

public:
	static CAchievment_DataLine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END