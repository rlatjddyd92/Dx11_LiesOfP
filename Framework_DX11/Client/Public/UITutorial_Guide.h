#pragma once

#include "Client_Defines.h"
#include "UIPage_Tutorial.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUITutorial_Guide : public CUIPage_Tutorial
{
public:



protected:
	CUITutorial_Guide(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUITutorial_Guide(const CUITutorial_Guide& Prototype);
	virtual ~CUITutorial_Guide() = default;

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

	void Set_Guide(vector<struct CUIPage::UIPART_INFO*>& vecOrigin);
	void Update_Guide(TUTO_INFO& NowData, _float fTimeDelta);

protected:
	UPART* m_pSharedPointer_Frame = { nullptr };
	UPART* m_pSharedPointer_Title = { nullptr };
	vector<UPART*> m_vecSharedPointer_Ctrl_Upper;
	vector<UPART*> m_vecSharedPointer_Ctrl_Middle;
	vector<UPART*> m_vecSharedPointer_Ctrl_Lower;

public:
	static CUITutorial_Guide* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END