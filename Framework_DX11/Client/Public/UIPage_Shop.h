#pragma once

#include "Client_Defines.h"
#include "UIPage.h"
#include "Item_Manager.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Shop : public CUIPage
{
public:
	enum class PART_GROUP
	{
		GROUP_FRAME,
		SHOP_Mouse_0,
		SHOP_Mouse_1,
		SHOP_Fx_0,
		SHOP_Fx_1,
		SHOP_Text_0,
		SHOP_Text_1,

		SHOP_Weight_Status_Text,
		SHOP_Weight_Stat,
		SHOP_Weight_Ratio,

		SHOP_Coin_Num,

		SHOP_Data_Area,
		SHOP_Title_Text,
		SHOP_Title_Line,
		SHOP_Cell_Array,
		SHOP_Cell_Static,
		SHOP_Cell_Fx,
		SHOP_Cell_Item,
		SHOP_Cell_Count,

		SHOP_Cell_Cost_Num,

		SHOP_Scroll_Line,
		SHOP_Scroll_Bar,

		SHOP_BoxInfo_Bag_Count_0,

		SHOP_BoxInfo_Bag_Count_1,
		SHOP_SELECT_Rect,
		SHOP_SELECT_Fire,


		GROUP_END
	};

	typedef struct SHOP_CELL_RENDER_INFO
	{
		_bool bIsTitle = false;
		_wstring strTitle = {};
		_Vec2 vPos = { 0.f,0.f };

		const CItem_Manager::ITEM* pInven = { nullptr };
		INVEN_ARRAY_TYPE eArray = INVEN_ARRAY_TYPE::TYPE_END;
		_int iInven_Index = 0;

		_int iIndexShop = -1;
	}CELL_RENDER;




protected:
	CUIPage_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Shop(const CUIPage_Shop& Prototype);
	virtual ~CUIPage_Shop() = default;

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

	

protected:
	void Action_Tab(_float fTimeDelta);
	void Action_Scroll(_float fTimeDelta);
	void Action_Cell(_float fTimeDelta);
	void Action_Focus(_float fTimeDelta, _Vec2 vPos);
	void Action_Popup(_float fTimeDelta);

	void Update_Tab(_float fTimeDelta);
	void Update_Scroll(_float fTimeDelta);
	void Update_Cell(_float fTimeDelta);
	void Update_Focus(_float fTimeDelta);
	void Update_BagInfo(_float fTimeDelta);

	void Setting_SellTab();
	void Setting_BuyTab();

	void Reset_PopupInfo();

	_int m_iNowTab = 0;
	_int m_iNowCell = 0;
	_int m_iNowCount = 0;
	_int m_iNowPrice = 0;

	_bool m_bIsItem_Popup = false;

	CELL_RENDER* m_Title_Bag = { nullptr };
	CELL_RENDER* m_Title_Chest = { nullptr };

	SCROLL* m_pScroll_Shop = { nullptr };

	vector<CELL_RENDER*> m_vecSell_RenderInfo;
	vector<CELL_RENDER*> m_vecBuy_RenderInfo;

public:
	static CUIPage_Shop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END