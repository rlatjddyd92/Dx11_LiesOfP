#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Main : public CUIPage
{
public:
	enum class PART_GROUP
	{
		MAIN_STATIC,
		MAIN_FX,
		MAIN_LOGO,
		MAIN_MOUSE,
		MAIN_TEXT_FX,
		MAIN_TEXT,
		GROUP_END
	};




protected:
	CUIPage_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Main(const CUIPage_Main& Prototype);
	virtual ~CUIPage_Main() = default;

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

	HRESULT Enter_Game();

protected:

	void Check_Mouse(_float fTimeDelta);

	void Main_Update(_float fTimeDelta);



	// 오프닝 제어 변수 
	_float m_fOpening_Progress = 0.f; // <- 1 도달할 때 까지 진행 

	// 마우스 반응 변수 
	_float m_fMouse_Effect_Ratio = 0.f;
	_float m_fRed_Effect_Ratio = 0.f;


public:
	static CUIPage_Main* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END