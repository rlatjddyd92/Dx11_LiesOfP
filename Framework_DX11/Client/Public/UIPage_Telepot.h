#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Telepot : public CUIPage
{
public:
	enum class PART_GROUP
	{
		TELEPOT_Back,
		TELEPOT_Fx_Cloud,
		TELEPOT_Fx_Grid,
		TELEPOT_List_Area,
		TELEPOT_Scroll_Line,
		TELEPOT_Scroll_Bar,
		TELEPOT_Dest_Frame,
		TELEPOT_Dest_Deco_LT,
		TELEPOT_Dest_Deco_Middle,
		TELEPOT_Dest_Deco_RB,
		TELEPOT_Dest_Picture_Start,
		TELEPOT_Dest_Picture_Rax,
		TELEPOT_Dest_Picture_Stair,
		TELEPOT_Dest_Picture_Manus,
		TELEPOT_Dest_Fx_Select,
		TELEPOT_Dest_Text_Now,
		TELEPOT_Dest_Text_Inactive,
		TELEPOT_Dest_Text_Name,
		GROUP_END
	};

	typedef struct TELEPOT_DEST_INFO
	{
		_uint iDestination_Navi_Index = -1;
		_wstring strDest_Name = {};
		_Vec2 vUICell_Pos = { 0.f,0.f };

		_bool bIsNow = false; // <- 현재 상호작용 중인 텔레포트
		_bool bIsInactive = false; // <- 비활성화 

	}DEST_INFO;



protected:
	CUIPage_Telepot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Telepot(const CUIPage_Telepot& Prototype);
	virtual ~CUIPage_Telepot() = default;

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

	_int Get_Telepot_Destination() { return m_vecTelepot_Dest[m_iNowActive]->iDestination_Navi_Index; }

protected:
	void Page_Setting();
	
	void Action_Scroll(_float fTimeDelta);
	void Action_Telepot(_float fTimeDelta);

	void Update_Telepot_Cell(_float fTimeDelta);


protected:
	_int m_iNowFocus = 0; 
	_int m_iNowActive = -1; // -1이면 작동하지 않은 상태

	vector<TELEPOT_DEST_INFO*> m_vecTelepot_Dest;

	_int m_iDest_Num = 4;
	SCROLL* m_pScroll_Telepot = { nullptr };
	_uint m_iNowNavi = -1;

public:
	static CUIPage_Telepot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END