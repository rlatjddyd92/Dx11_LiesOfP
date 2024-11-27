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
	// æ∆¿Ã≈€ Focusing 
	

protected:








public:
	static CUIPage_ItemInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END