#pragma once

#include "Client_Defines.h"
#include "UIPage.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPage_Menu : public CUIPage
{
public:
	enum class PART_GROUP
	{





		GROUP_END
	};




protected:
	CUIPage_Menu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage_Menu(const CUIPage_Menu& Prototype);
	virtual ~CUIPage_Menu() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void OpenAction() override;
	virtual void CloseAction() override;

public:
	const vector<UPART*>& Get_UIPartInfo() { return m_vecPart; }
	virtual HRESULT Ready_UIPart_Group_Control() override;

protected:








public:
	static CUIPage_Menu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;




};

END