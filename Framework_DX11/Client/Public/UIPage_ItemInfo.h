#pragma once

#include "Client_Defines.h"
#include "UIPage.h"
#include "Item_Manager.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_ItemInfo : public CUIPage
{
public:
	enum class PART_GROUP
	{
		ITEMINFO_TOOLTIP_Frame,
		ITEMINFO_SELECT_Rect,
		ITEMINFO_SELECT_Fire,
		ITEMINFO_NEW,
		ITEMINFO_ACTION_Header,
		ITEMINFO_ACTION_Back,
		ITEMINFO_ACTION_Deco,
		ITEMINFO_ACTION_Mouse_0,
		ITEMINFO_ACTION_Fx_0,
		ITEMINFO_ACTION_Text_0,
		ITEMINFO_ACTION_Mouse_1,
		ITEMINFO_ACTION_Fx_1,
		ITEMINFO_ACTION_Text_1,
		ITEMINFO_ACTION_Mouse_2,
		ITEMINFO_ACTION_Fx_2,
		ITEMINFO_ACTION_Text_2,
		ITEMINFO_ACTION_Mouse_3,
		ITEMINFO_ACTION_Fx_3,
		ITEMINFO_ACTION_Text_3,


		TOOLTIP_Back,
		TOOLTIP_Fx0,
		TOOLTIP_Fx1,
		TOOLTIP_Fx2,
		TOOLTIP_Fx3,
		TOOLTIP_Fx4,
		TOOLTIP_Fx5,
		TOOLTIP_Item_Frame,
		TOOLTIP_Item_Fx0,
		TOOLTIP_Item_Fx1,
		TOOLTIP_Item_Blade,
		TOOLTIP_Item_Cross,
		TOOLTIP_Item_Handle,
		TOOLTIP_Item_Heroic,
		TOOLTIP_Item_Normal,
		TOOLTIP_Item_Name,
		TOOLTIP_Item_Desc,
		TOOLTIP_Item_ShopInfo_Frame,
		TOOLTIP_Item_ShopInfo_Title,
		TOOLTIP_Item_ShopInfo_Ergo,
		TOOLTIP_Item_ShopInfo_Count,
		TOOLTIP_Item_ChestInfo_Frame,
		TOOLTIP_Item_ChestInfo_Desc,

		DROP_Frame,
		DROP_Item_Fx,
		DROP_Frame_Stripe,
		DROP_W0,
		DROP_W1,
		DROP_Item_Icon,
		DROP_Item_Type_Frame,
		DROP_Item_Type_Symbol,
		DROP_Item_H0,
		DROP_Item_Name,



		GROUP_END
	};

	typedef struct DROP_ITEM_INFO
	{
		_int iIndex = -1;
		_int iCount = 0;
		_float fLifeTime = 0.f;
	}DROP;



protected:
	CUIPage_ItemInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_ItemInfo(const CUIPage_ItemInfo& Prototype);
	virtual ~CUIPage_ItemInfo() = default;

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

public:
	void Set_Active_ItemInfo(_bool bIsActive, UIPAGE eNowPage) { m_vecPageAction[_int(PAGEACTION::ACTION_ACTIVE)] = bIsActive; m_eNowPage = eNowPage; }
	void Off_ItemInfo_UI()
	{
		Off_Focus();
		Off_ItemAction();
	}

	void Show_Focus(_Vec2 vItemCellPos, _Vec2 vItemCellSize);
	void Off_Focus();

	void Show_NewMark(_Vec2 vItemCellPos, _Vec2 vItemCellSize);

	void Show_ItemAction(_Vec2 vItemCellPos, _Vec2 vItemCellSize, ITEM_FUNC eFunc0, ITEM_FUNC eFunc1 = ITEM_FUNC::FUNC_END, ITEM_FUNC eFunc2 = ITEM_FUNC::FUNC_END, ITEM_FUNC eFunc3 = ITEM_FUNC::FUNC_END);
	void Off_ItemAction();

	void Action_ItemAction(_float fTimeDelta);


	ITEM_FUNC Get_Active_Func()
	{
		if (m_iNow_Func == -1)
			return ITEM_FUNC::FUNC_END;
		else
			return m_eActive_Func[m_iNow_Func];
	}

	// 툴팁 관련 사항 
	void Show_Tooltip_Shop(_int iShopIndex);
	void Show_Tooltip_Chest(_int iChestIndex);

	void Show_Tooltip(INVEN_ARRAY_TYPE eType, _int iIndex);
	void Show_Tooltip(EQUIP_SLOT eSlot);

	void Input_Drop_Item_Info(_int iIndex, _int iCount)
	{
		DROP_ITEM_INFO* pNew = new DROP_ITEM_INFO;
		pNew->iIndex = iIndex;
		pNew->iCount = iCount;
		pNew->fLifeTime = m_fDrop_Item_ShowTime;
		m_DropItem_Index_list.push_back(pNew);
		if (m_DropItem_Index_list.size() > m_iDrop_Item_Render_Limit)
			while (m_DropItem_Index_list.size() > m_iDrop_Item_Render_Limit)
			{
				Safe_Delete(m_DropItem_Index_list.front());
				m_DropItem_Index_list.pop_front();
			}
	}


protected:
	void Make_TooltipPage(const CItem_Manager::ITEM* Item, _bool bIsWeapon = false, const CItem_Manager::ITEM* ItemHandle = nullptr);

	// Focus
	_bool m_bFocus = false;
	_float m_fFocus_Fire_Move_Ratio = 0.f;

	// New
	queue<_Vec2> m_queueNewMarkPos;
	_float m_fNewmarkGrow = 0.f;

	// Func
	_bool m_bIsActive_Func = false;
	_int m_iNow_Func = -1;
	ITEM_FUNC m_eActive_Func[4] = { ITEM_FUNC::FUNC_END, ITEM_FUNC::FUNC_END ,ITEM_FUNC::FUNC_END ,ITEM_FUNC::FUNC_END };
	_wstring m_strFuncName[_int(ITEM_FUNC::FUNC_END)];

	UIPAGE m_eNowPage = UIPAGE::PAGE_END;

	_bool m_bCheck_Active = false;

	// Tooltip
	_Vec2 m_vToolTip_Pos = { 1000.f,360.f };

	// 드랍
	void Add_Render_Info_DropInfo(_float fTimeDelta);

	list<DROP_ITEM_INFO*> m_DropItem_Index_list;
	_int m_iDrop_Item_Render_Limit = 3;
	_float m_fEmerge_Effect_Time = 1.f;
	_float m_fDrop_Item_ShowTime = 3.f;





public:
	static CUIPage_ItemInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END