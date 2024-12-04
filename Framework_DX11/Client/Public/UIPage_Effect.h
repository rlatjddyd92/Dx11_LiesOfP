#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Effect : public CUIPage
{
public:
	enum class PART_GROUP
	{
		EFFECT_Center,
		EFFECT_FIFO_Screen,
		EFFECT_FIFO_Title,
		EFFECT_FIFO_Text,
		EFFECT_SCRIPT_Back,
		EFFECT_SCRIPT_Text0,
		EFFECT_SCRIPT_Text1,

		GROUP_END
	};




protected:
	CUIPage_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Effect(const CUIPage_Effect& Prototype);
	virtual ~CUIPage_Effect() = default;

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

public:
	_bool Fade_Out(_wstring strTitle, _wstring strDesc, _Vec3 vColor = _Vec3{ 0.f,0.f,0.f }, _float fTime = 1.f);
	_bool Fade_In(_float fTime = 1.f);

	void Show_Script(_wstring strScript0, _wstring strScript1 = TEXT("none"), _float fTime = 1.f, _Vec3 vColor = _Vec3{ 0.f,0.f,0.f });

	_float Check_Fade() { return m_fTime_Fade_Now / m_fTime_Fade_Max; }
	_float Check_Script() { return m_fTime_Script_Now / m_fTime_Script_Max; }


private:
	_float m_fTime_Fade_Now = 0.f;
	_float m_fTime_Fade_Max = 0.f;
	_bool m_bFadeOut = false;
	_float m_fTime_Script_Now = 0.f;
	_float m_fTime_Script_Max = 0.f;


public:
	static CUIPage_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END