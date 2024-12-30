#pragma once

#include "Client_Defines.h"
#include "UIPage_Play.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPlay_Component : public CUIPage_Play
{
protected:
	CUIPlay_Component(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPlay_Component(const CUIPlay_Component& Prototype);
	virtual ~CUIPlay_Component() = default;

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


private:
	_Vec2 m_vComponent_Pos = { 0.f,0.f };










public:
	static CUIPlay_Component* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END

