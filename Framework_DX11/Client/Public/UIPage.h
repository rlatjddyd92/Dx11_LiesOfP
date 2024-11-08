#pragma once

#include "Client_Defines.h"

#include "UIObject.h"

class CUIPage : public CUIObject
{
protected:
	CUIPage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPage(const CUIPage& Prototype);
	virtual ~CUIPage() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;






public:
	static CUIPage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

