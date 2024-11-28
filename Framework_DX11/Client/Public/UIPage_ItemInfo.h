#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

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
		GROUP_END
	};





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

	void Show_Focus(_Vec2 vItemCellPos, _Vec2 vItemCellSize);
	void Off_Focus(_Vec2 vItemCellPos, _Vec2 vItemCellSize);

	void Show_NewMark(_Vec2 vItemCellPos, _Vec2 vItemCellSize);

	void Show_ItemAction(_Vec2 vItemCellPos, _Vec2 vItemCellSize, ITEM_FUNC eFunc0, ITEM_FUNC eFunc1 = ITEM_FUNC::FUNC_END, ITEM_FUNC eFunc2 = ITEM_FUNC::FUNC_END, ITEM_FUNC eFunc3 = ITEM_FUNC::FUNC_END);
	void Off_ItemAction();

	void Action_ItemAction(_float fTimeDelta);


protected:
	_bool m_bFocus = false;
	_float m_fFocus_Fire_Move_Ratio = 0.f;

	queue<_Vec2> m_queueNewMarkPos;

	ITEM_FUNC m_eActive_Func[4] = { ITEM_FUNC::FUNC_END, ITEM_FUNC::FUNC_END ,ITEM_FUNC::FUNC_END ,ITEM_FUNC::FUNC_END };
	_wstring m_strFuncName[_int(ITEM_FUNC::FUNC_END)];

	UIPAGE m_eNowPage = UIPAGE::PAGE_END;

public:
	static CUIPage_ItemInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END