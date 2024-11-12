#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CTrail_TwoPoint_Instance final : public CVIBuffer_Instancing
{
public:
	typedef struct
	{
		_float3 vTop;
		_float3 vBottom;
	}TWOPOINT;

private:
	CTrail_TwoPoint_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_TwoPoint_Instance(const CTrail_TwoPoint_Instance& Prototype);
	virtual ~CTrail_TwoPoint_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient);
	virtual HRESULT Initialize(void* pArg) override;

	_bool Update_Buffer(_fvector vWorldTopPos, _fvector vWorldBottomPos, _float fTimeDelta);
	void Reset();

private:
	list<TWOPOINT> m_TrailPoses;
	_bool m_bFirst = { false };

public:
	static CTrail_TwoPoint_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END