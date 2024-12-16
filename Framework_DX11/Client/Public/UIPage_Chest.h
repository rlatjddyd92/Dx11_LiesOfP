#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Chest : public CUIPage
{
public:
	enum class PART_GROUP
	{
		CHEST_Back,
		CHEST_Top,
		CHEST_Fx,
		CHEST_Title,
		CHEST_Line,

		CHEST_Weight_UnderLine,
		CHEST_Weight_Status_Frame,
		CHEST_Weight_Status_Text,
		CHEST_Weight_Symbol,
		CHEST_Weight_Stat,
		CHEST_Weight_Ratio,


		CHEST_Inven_Text,
		CHEST_Inven_Line,

		CHEST_Inven_Data_Area,

		CHEST_Inven_Array,
		CHEST_Inven_Array_Cell0_Frame,
		CHEST_Inven_Array_Cell0_Fx,
		CHEST_Inven_Array_Cell0_Item,
		CHEST_Inven_Array_Cell0_Count,
		CHEST_Inven_X,
		CHEST_Inven_Array_Cell0_Equip_Symbol,

		CHEST_Inven_Scroll_Line,
		CHEST_Inven_Bar,

		CHEST_Arrow,


		CHEST_Dest_Text,
		CHEST_Dest_Line,

		CHEST_Dest_Data_Area,

		CHEST_Dest_Array,
		CHEST_Dest_Cell0_Frame,
		CHEST_Dest_Cell0_Fx,
		CHEST_Dest_Cell0_Item,
		CHEST_Dest_Cell0_Count,
		CHEST_Dest_X,
		CHEST_Dest_Cell0_Equip_Symbol,

		CHEST_Dest_Scroll_Line,
		CHEST_Dest_Bar,


		CHEST_BoxInfo_Frame,
		CHEST_BoxInfo_H0,
		CHEST_BoxInfo_H1,
		CHEST_BoxInfo_W0,
		CHEST_BoxInfo_W1,
		CHEST_BoxInfo_Line,
		CHEST_BoxInfo_Bag_Symbol0,
		CHEST_BoxInfo_Bag_Text0,
		CHEST_BoxInfo_Bag_Count0,
		CHEST_BoxInfo_Bag_Symbol1,
		CHEST_BoxInfo_Bag_Text1,
		CHEST_BoxInfo_Bag_Count1,



		GROUP_END
	};

	typedef struct CHEST_CELL_RENDER_INFO
	{
		_bool bIsInven = false;

		const CItem_Manager::ITEM* pInven = { nullptr };
		INVEN_ARRAY_TYPE eArray = INVEN_ARRAY_TYPE::TYPE_END;
		_int iInven_Index = 0;

		_int iIndexChest = -1;

		_Vec2 vPos = { 0.f,0.f };
	}CELL_RENDER;


protected:
	CUIPage_Chest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Chest(const CUIPage_Chest& Prototype);
	virtual ~CUIPage_Chest() = default;

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

	void Setting_Cell();

	void Reset_PopupInfo();

	_int m_iNowTab = 0;
	_int m_iNowCell = 0;
	_int m_iNowCount = 0;

	_bool m_bIsItem_Popup = false;
	SCROLL* m_pScroll_InvenInfo = { nullptr };
	SCROLL* m_pScroll_Chest = { nullptr };

	vector<CELL_RENDER*> m_vecSell_RenderInfo;
	vector<CELL_RENDER*> m_vecBuy_RenderInfo;








public:
	static CUIPage_Chest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END