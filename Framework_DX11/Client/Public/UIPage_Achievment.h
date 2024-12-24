#pragma once

#include "Client_Defines.h"
#include "UIPage.h"



BEGIN(Engine)

END

BEGIN(Client)
class CStatistic_Data;
class CAchievment_DataLine;

class CUIPage_Achievment : public CUIPage
{
public:
	enum class PART_GROUP
	{
		GROUP_FRAME,
		GROUP_TITLE,
		GROUP_STATISTIC_FRAME,
		GROUP_STATISTIC_LINE,
		GROUP_GOAL_FRAME,
		GROUP_GOAL_LINE,
		GROUP_GOAL_ICON,
		GROUP_GOAL_TITLE,
		GROUP_GOAL_DESC,
		GROUP_GOAL_COUNT,
		GROUP_GOAL_RESULT,
		GROUP_POPUP_FRAME,
		GROUP_POPUP_ICON,
		GROUP_POPUP_TITLE,
		GROUP_POPUP_DESC,
		GROUP_END
	};


protected:
	CUIPage_Achievment(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Achievment(const CUIPage_Achievment& Prototype);
	virtual ~CUIPage_Achievment() = default;

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

	void Input_Achievment_Data(_int iIndex, _int Data);
	

protected:
	void Make_Achievment_DataLine();

	void Action_Slide(_float fTimeDelta);

	void Check_Data(_float fTimeDelta);

	void Update_Static(_float fTimeDelta);
	void Update_Statistic(_float fTimeDelta);
	void Update_Achievment(_float fTimeDelta);
	void Update_Popup(_float fTimeDelta);

protected:
	vector<class CStatistic_Data*> m_vecStatistic;
	vector<class CAchievment_DataLine*> m_vecAcievment;

	SCROLL* m_pScroll = { nullptr };



	// ÆË¾÷ Á¶Àý¿ë 
	_int m_iMax_Popup = 4;
	list<_float> m_Popup_Ratiolist;
	list<_int> m_Popup_Indexlist;

public:
	static CUIPage_Achievment* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END