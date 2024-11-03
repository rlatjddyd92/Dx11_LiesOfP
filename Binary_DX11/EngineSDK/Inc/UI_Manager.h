#pragma once
#include "Base.h"

BEGIN(Engine)
class CUIObject;

class CUI_Manager final
	: public CBase
{
private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT Initialize();
	void	Update();

public:
	void		Set_FocusUI(CUIObject* _pUI);

	CUIObject*	Get_FocusUI();
	CUIObject*	Get_TargetUI(CUIObject* _pFocusUI);

public:
	class CGameInstance* m_pGameInstance = { nullptr };

	CUIObject* m_pFocusUI = { nullptr };

public:
	static CUI_Manager* Create();
	virtual void Free() override;


};

END
