#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Loading : public CUIPage
{
public:
	enum class PART_GROUP
	{
		GROUP_CREADIT_STATIC = 4,
		GROUP_CREADIT_LEADER,
		GROUP_CREADIT_NAME_FX,
		GROUP_CREADIT_NAME,
		GROUP_CREADIT_ROLE_0,
		GROUP_CREADIT_ROLE_1,

		GROUP_LOADING_STATIC,
		GROUP_LOADING_TEXT_0,
		GROUP_LOADING_TEXT_1,
		GROUP_LOADING_TEXT_2,
		GROUP_LOADING_NOSE,
		GROUP_LOADING_NOSE_TEXT,
		GROUP_LOADING_NOSE_PROGRESS,

		GROUP_END
	};

	typedef struct CREADIT_CTRL
	{
		_wstring strTitle = {};
		_wstring strName = {};
		_wstring strRole_First = {};
		_wstring strRole_Second = {};
	}CREADIT;


protected:
	CUIPage_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Loading(const CUIPage_Loading& Prototype);
	virtual ~CUIPage_Loading() = default;

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

	void Set_Loading_Status(_wstring strMessage, _float fProgress)
	{
		m_fLoading_Progress = min(fProgress, 1.f);
		m_fLoading_Progress = max(m_fLoading_Progress, 0.f);
		m_strLoading_Message = strMessage;
	}

public:
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }
	virtual HRESULT Ready_UIPart_Group_Control() override;

protected:
	void Credit_Update(_float fTimeDelta);
	void Loading_Update(_float fTimeDelta);

	_float m_fLoading_Progress = 0.f;
	_wstring m_strLoading_Message = {};

	vector<CREADIT_CTRL*> m_vecCreadit;

	_int m_iCreadit_Count = 0;
	_bool m_bLoading_Scene = false;

	_float m_fShowTime_Now = 0.f;
	_float m_fShowTime_Max = 3.f;

	_float m_fAppear = 0.f;

	_float m_fInverval = 0.f;

public:
	static CUIPage_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END