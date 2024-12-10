#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_2DPolygon final : public CVIBuffer
{
public:
	typedef struct
	{
		_int iPoint = 0;
		_float fAngle = 0.f;
	}UIPOLIGON_DESC;

private:
	CVIBuffer_2DPolygon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_2DPolygon(const CVIBuffer_2DPolygon& Prototype);
	virtual ~CVIBuffer_2DPolygon() = default;

public:
	virtual HRESULT Initialize_Prototype(const UIPOLIGON_DESC& Desc);
	virtual HRESULT Initialize(void* pArg);

	void Set_Point_Ratio(_float fRatio, _int iIndex);

private:
	_Vec4 Make_Vertices(_float fAngle);


	_int m_iPoint = 0;
	_float m_fTurn = 0.f;


	vector<_Vec4> m_vecOrigin_Vertex;



public:
	static CVIBuffer_2DPolygon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const UIPOLIGON_DESC& Desc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END