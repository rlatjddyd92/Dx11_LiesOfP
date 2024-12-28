#pragma once

#include "Client_Defines.h"
#include "UIPage_Tutorial.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUITutorial_Result : public CUIPage_Tutorial
{
public:



protected:
	CUITutorial_Result(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUITutorial_Result(const CUITutorial_Result& Prototype);
	virtual ~CUITutorial_Result() = default;

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
	static CUITutorial_Result* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END