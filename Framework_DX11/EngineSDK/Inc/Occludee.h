#pragma once
#include "Base.h"

BEGIN(Engine)

class COccludee :
    public CBase
{
private:
	COccludee(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~COccludee() = default;

public:
	HRESULT Initialize();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static COccludee* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END