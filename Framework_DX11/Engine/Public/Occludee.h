#pragma once
#include "Base.h"

BEGIN(Engine)

class COccludee :
    public CBase
{
private:
	COccludee(ID3D11Device* pDevice);
	virtual ~COccludee() = default;

public:
	HRESULT Initialize();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDefferedContext = { nullptr };
	D3D11_VIEWPORT m_Hiz_VP;
public:
	static COccludee* Create(ID3D11Device* pDevice);
	virtual void Free() override;

};

END