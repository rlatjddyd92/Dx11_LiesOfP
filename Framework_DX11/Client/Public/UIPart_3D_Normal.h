#pragma once

#include "Client_Defines.h"
#include "UIPart_3D.h"
#include "GameInstance.h"

BEGIN(Engine)

END

BEGIN(Client)

class CUIPart_3D_Normal : public CUIPart_3D
{
protected:
	CUIPart_3D_Normal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIPart_3D_Normal(const CUIPart_3D& Prototype);
	virtual ~CUIPart_3D_Normal() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CUIPart_3D_Normal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END