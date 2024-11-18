#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Menu : public CUIPage
{
public:
	enum class PART_GROUP
	{
		GROUP_BACK,
		GROUP_MENU_SYMBOL,
		GROUP_MENU_TITLE,

		GROUP_TOP_SYMBOL,
		GROUP_TOP_TITLE,

		GROUP_BOTTOM_SYMBOL,
		GROUP_BOTTOM_TITLE,

		GROUP_BAG_SYMBOL,
		GROUP_BAG_TITLE,

		GROUP_DESC_BACK,
		GROUP_DESC_TITLE,
		GROUP_DESC_TEXT,
		GROUP_DESC_FX,
		GROUP_DESC_BUTTON,
		GROUP_DESC_MOUSE,

		GROUP_MENU_EQUIP,
		GROUP_MENU_INVEN,
		GROUP_MENU_CHARACTOR,
		GROUP_MENU_HEART,
		GROUP_MENU_OPTION,

		GROUP_TOP_CELL_0, // 위 벨트 0번에 대한 것 수행 (ItemManager에서 정보 받기 -> 백판,셀 심벌ON/OFF, 아이템 정보 ON/OFF, 마우스 감지 -> 셀렉트 요청)
		GROUP_TOP_CELL_1,
		GROUP_TOP_CELL_2,

		GROUP_BOTTOM_CELL_0,
		GROUP_BOTTOM_CELL_1,
		GROUP_BOTTOM_CELL_2,

		GROUP_BAG_CELL_0,
		GROUP_BAG_CELL_1,
		GROUP_BAG_CELL_2,
		GROUP_BAG_CELL_3,

		GROUP_ITEM_DESC_MOUSE_0,
		GROUP_ITEM_DESC_MOUSE_1,
		GROUP_ITEM_DESC_BACK,
		GROUP_ITEM_DESC_NAME,
		GROUP_ITEM_DESC_FUNC_0,
		GROUP_ITEM_DESC_FUNC_1,

		GROUP_SELECT_MARK,

		GROUP_END
	};




protected:
	CUIPage_Menu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Menu(const CUIPage_Menu& Prototype);
	virtual ~CUIPage_Menu() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OpenAction() override;
	virtual void CloseAction() override;

	virtual CHECK_MOUSE Check_Mouse_By_Part_In_Page() override;

public:
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }
	virtual HRESULT Ready_UIPart_Group_Control() override;

private:
	void Focus_Update(); // 메뉴 화면에서 현재 하이라이트 된 


protected:
	PART_GROUP m_eFocus_Group = PART_GROUP::GROUP_END;








public:
	static CUIPage_Menu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END