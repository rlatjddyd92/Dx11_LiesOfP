#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Popup : public CUIPage
{
public:
	enum class PART_GROUP
	{
		POPUP_Top,
		POPUP_Middle,
		POPUP_Bottom,
		POPUP_Title,
		POPUP_Desc_0,
		POPUP_Desc_1,
		POPUP_Mouse,
		POPUP_Space,
		POPUP_Text,





		GROUP_END
	};




protected:
	CUIPage_Popup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Popup(const CUIPage_Popup& Prototype);
	virtual ~CUIPage_Popup() = default;

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
	void Show_Popup(_wstring strTitle, _wstring strDescA, _wstring strDescB = TEXT("none"));
	void Off_Popup();

public:
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }
	virtual HRESULT Ready_UIPart_Group_Control() override;

protected:








public:
	static CUIPage_Popup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END