#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Equip : public CUIPage
{
public:
	enum class PART_GROUP
	{
		GROUP_FRAME,
		GROUP_MODEL,
		GROUP_TOP_STATIC,
		GROUP_TOP_TAP_HIGHLIGHT_LINE,
		GROUP_TOP_TAP_BUTTON_0,
		GROUP_TOP_TAP_BUTTON_1,
		GROUP_TOP_TAP_BUTTON_2,

		GROUP_TOP_WEIGHT_STATUS,
		GROUP_TOP_WEIGHT_STAT,
		GROUP_TOP_WEIGHT_RATIO,

		GROUP_TOP_COIN_COUNT,

		// Page0
		GROUP_PAGE_0_FRAME,

		GROUP_PAGE_0_WEAPON_SYMBOL,
		GROUP_PAGE_0_AMULET_SYMBOL,
		GROUP_PAGE_0_DEFENCE_SYMBOL,

		GROUP_PAGE_0_WEAPON_TEXT,
		GROUP_PAGE_0_AMULET_TEXT,
		GROUP_PAGE_0_DEFENCE_TEXT,

		GROUP_PAGE_0_WEAPON_FIRST,
		GROUP_PAGE_0_WEAPON_SECOND,

		GROUP_PAGE_0_ARM,

		GROUP_PAGE_0_AMULET_FIRST,
		GROUP_PAGE_0_AMULET_SECOND,

		GROUP_PAGE_0_DEFENCE_0,
		GROUP_PAGE_0_DEFENCE_1,
		GROUP_PAGE_0_DEFENCE_2,
		GROUP_PAGE_0_DEFENCE_3,

		// Page1
		GROUP_PAGE_1_FRAME,

		GROUP_PAGE_1_TOP_SYMBOL,
		GROUP_PAGE_1_BOTTOM_SYMBOL,
		GROUP_PAGE_1_BAG_SYMBOL,

		GROUP_PAGE_1_TOP_TEXT,
		GROUP_PAGE_1_BOTTOM_TEXT,
		GROUP_PAGE_1_BAG_TEXT,

		GROUP_PAGE_1_TOP_0,
		GROUP_PAGE_1_TOP_1,
		GROUP_PAGE_1_TOP_2,

		GROUP_PAGE_1_BOTTOM_0,
		GROUP_PAGE_1_BOTTOM_1,
		GROUP_PAGE_1_BOTTOM_2,

		GROUP_PAGE_1_BAG_0,
		GROUP_PAGE_1_BAG_1,
		GROUP_PAGE_1_BAG_2,
		GROUP_PAGE_1_BAG_3,

		// Page2
		GROUP_PAGE_2_FRAME,

		GROUP_PAGE_2_HEAD_SYMBOL,
		GROUP_PAGE_2_COSTUME_SYMBOL,

		GROUP_PAGE_2_HEAD_TEXT,
		GROUP_PAGE_2_COSTUME_TEXT,

		GROUP_PAGE_2_HEAD_0,
		GROUP_PAGE_2_HEAD_1,
		
		GROUP_PAGE_2_COSTUME,

		GROUP_SELECT_FRAME,
		GROUP_SELECT_EFFECT,

		GROUP_END
	};




protected:
	CUIPage_Equip(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Equip(const CUIPage_Equip& Prototype);
	virtual ~CUIPage_Equip() = default;

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








public:
	static CUIPage_Equip* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END