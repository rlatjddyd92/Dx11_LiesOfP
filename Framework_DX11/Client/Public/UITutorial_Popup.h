#pragma once

#include "Client_Defines.h"
#include "UIPage_Tutorial.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUITutorial_Popup : public CUIPage_Tutorial
{
public:



protected:
	CUITutorial_Popup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUITutorial_Popup(const CUITutorial_Popup& Prototype);
	virtual ~CUITutorial_Popup() = default;

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

	void Set_Popup(vector<struct CUIPage::UIPART_INFO*>& vecOrigin);
	void Update_Popup(TUTO_INFO& NowData, _float fTimeDelta);

protected:
	vector<UPART*> m_vecSharedPointer_Frame;
	UPART* m_pSharedPointer_Title = { nullptr };
	UPART* m_pSharedPointer_Score = { nullptr };








public:
	static CUITutorial_Popup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END