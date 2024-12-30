#pragma once

#include "Client_Defines.h"
#include "UIPage_Tutorial.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUITutorial_Info : public CUIPage_Tutorial
{
public:
	typedef struct
	{
		_wstring m_strTitle = {};
		_int m_iNow = 0;
		_int m_iGoal = 0;
		_int m_iScore = 0;
	}TUTO_INFO;

protected:
	CUITutorial_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUITutorial_Info(const CUITutorial_Info& Prototype);
	virtual ~CUITutorial_Info() = default;

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

	void Set_Info(vector<struct CUIPage::UIPART_INFO*>& vecOrigin);
	void Update_Info(TUTO_INFO& NowData, _float fTimeDelta);

protected:
	vector<UPART*> m_vecSharedPointer_All;
	UPART* m_pSharedPointer_Mission_Title = { nullptr };
	UPART* m_pSharedPointer_Mission_Count = { nullptr };
	UPART* m_pSharedPointer_Mission_Score = { nullptr };
	UPART* m_pSharedPointer_Sum = { nullptr };

	_int m_iScore_Sum = 0;

public:
	static CUITutorial_Info* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END