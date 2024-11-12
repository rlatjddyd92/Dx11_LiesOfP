#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL Trail_TwoPoint_Instance final : public CVIBuffer_Instancing
{
public:
	typedef struct
	{
		_float3 vTop;
		_float3 vBottom;
	}TWOPOINT;

private:
	Trail_TwoPoint_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Trail_TwoPoint_Instance(const Trail_TwoPoint_Instance& Prototype);
	virtual ~Trail_TwoPoint_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc);
	virtual HRESULT Initialize(void* pArg) override;

	_bool Update_Buffer(_fvector vWorldTopPos, _fvector vWorldBottomPos, _float fTimeDelta);

private:
	list<TWOPOINT> m_TrailPoses;

public:
	static Trail_TwoPoint_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END