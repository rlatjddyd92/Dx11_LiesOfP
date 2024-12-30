#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "GameInstance.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPart_3D : public CGameObject
{
protected:
	CUIPart_3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPart_3D(const CUIPart_3D& Prototype);
	virtual ~CUIPart_3D() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CUIPart_3D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END