#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Achievment : public CUIPage
{
public:
	enum class PART_GROUP
	{
		GROUP_FRAME,
		GROUP_TITLE,
		GROUP_STATISTIC_FRAME,
		GROUP_STATISTIC_LINE,
		



		GROUP_END
	};

	// 필요한 것 

	/*
	1. 배경판 + 타이틀 세팅 
	2. 스크롤 조정 
	3. 통계판 조정 
	 3-1. 통계판 내용 조정
	4. 업적판 조정 
	 4-1. 업적판 내용 조정 
	*/








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

protected:








public:
	static CUIPage_Achievment* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END