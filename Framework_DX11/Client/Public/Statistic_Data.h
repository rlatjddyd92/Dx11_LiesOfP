#pragma once

#include "Client_Defines.h"
#include "UIPage_Achievment.h"


BEGIN(Engine)

END

BEGIN(Client)

class CStatistic_Data : public CUIPage_Achievment
{
protected:
	CStatistic_Data(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatistic_Data(const CStatistic_Data& Prototype);
	virtual ~CStatistic_Data() = default;

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

	void Set_Statistic_Data(vector<struct CUIPage::UIPART_INFO*>& vecOrigin, _int iIndex);

	void Input_Data_Playtime(_float fTimeDelta);
	void Input_Data_AttackDamege(_float fDamege);
	void Add_Data_KillCount();
	void Input_Data_ErgoGain(_int iCount);

	void Update_Statistic_Line(_float fAlpha);

protected:
	_Vec2 m_vParent = { 0.f,0.f };

	_float m_fRatio_Data = 0.f;
	_wstring m_strTitle = {};
	_wstring m_strStat = {};

	_int m_iStaticIndex = -1;

	_float m_fPlayTime = 0.f;
	_float m_fAttackDamege = 0.f;
	_int m_iKillCount = 0;
	_int m_iEgroGainCount = 0;

	vector<struct CUIPage::UIPART_INFO*> m_vecStatic;
	struct CUIPage::UIPART_INFO* m_pTitle = { nullptr };
	struct CUIPage::UIPART_INFO* m_pStat = { nullptr };

public:
	static CStatistic_Data* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END
