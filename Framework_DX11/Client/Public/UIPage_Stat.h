#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Stat : public CUIPage
{
public:
	enum class PART_GROUP
	{
		GROUP_STATIC,
		GROUP_POINT_BUTTON_0,
		GROUP_POINT_BUTTON_1,
		GROUP_POINT_BUTTON_2, 
		GROUP_POINT_BUTTON_3,
		GROUP_POINT_BUTTON_4,
		GROUP_POINT_NUM_0,
		GROUP_POINT_NUM_1,
		GROUP_POINT_NUM_2,
		GROUP_POINT_NUM_3,
		GROUP_POINT_NUM_4,

		GROUP_ERGO_NEED,
		GROUP_ERGO_NOW,
		GROUP_ERGO_DEPOSIT_TEXT,
		GROUP_ERGO_DEPOSIT_NUM_0,
		GROUP_ERGO_DEPOSIT_NUM_1,

		GROUP_LEVELUP_BUTTON,
		GROUP_LEVELUP_FX,
		GROUP_LEVELUP_TEXT,

		GROUP_SPEC_0,
		GROUP_SPEC_1,
		GROUP_SPEC_2,
		GROUP_SPEC_3,
		GROUP_SPEC_4,
		GROUP_SPEC_5,
		GROUP_SPEC_6,
		GROUP_SPEC_7,
		GROUP_SPEC_8,

		GROUP_ABILITY_6,
		GROUP_ABILITY_7,
		GROUP_ABILITY_8,

		GROUP_SELECT_RECT,
		GROUP_SELECT_FIRE,

		GROUP_POINT_RECT_0,
		GROUP_POINT_RECT_1,
		GROUP_POINT_RECT_2,
		GROUP_POINT_RECT_3,
		GROUP_POINT_RECT_4,

		GROUP_STARCHART_STATIC,
		GROUP_STARCHART_LEVELUP,
		GROUP_STARCHART_ORIGIN,

		GROUP_END
	};



protected:
	CUIPage_Stat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Stat(const CUIPage_Stat& Prototype);
	virtual ~CUIPage_Stat() = default;

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

	void Action_Point(_float fTimeDelta);
	void Action_LevelUp(_float fTimeDelta);
	void Action_Focus(_float fTimeDelta);

protected:
	void Update_Point(_float fTimeDelta);
	void Update_LevelUp(_float fTimeDelta);
	void Update_Focus(_float fTimeDelta);
	void Update_SpecData(_float fTimeDelta);
	void Update_StarChart(_float fTimeDelta);

	void Input_LevelUp_Stat();

private:
	_int m_iFocus_Point = -1;
	_int m_iLevelUp_Buffer_Point[5] = { 0, };
	_float m_iLevelUp_Buffer_Stat[9] = { 0.f, };
	_int m_iUsing_Point_Now = 0;

	_bool m_bActive_LevelUp_Button = false;

public:
	static CUIPage_Stat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END