#pragma once

#include "Client_Defines.h"
#include "UIPage_Tutorial.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUITutorial_Timing : public CUIPage_Tutorial
{
public:



protected:
	CUITutorial_Timing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUITutorial_Timing(const CUITutorial_Timing& Prototype);
	virtual ~CUITutorial_Timing() = default;

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

	void Set_Timing(vector<struct CUIPage::UIPART_INFO*>& vecOrigin);

	void Start_Timing(KEY eKey, _float fTime);
	_bool Update_Timing(_float fTimeDelta);

protected:
	UPART* m_pSharedPointer_Frame = { nullptr };
	UPART* m_pSharedPointer_Gauge = { nullptr };
	UPART* m_pSharedPointer_Icon = { nullptr };

	_Vec2 m_vTime = { 0.f,0.f };
	_bool m_bActive = false;

public:
	static CUITutorial_Timing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END