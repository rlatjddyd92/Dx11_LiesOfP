#pragma once

#include "VIBuffer_Instancing.h"


BEGIN(Engine)
class ENGINE_DLL CTrail_OnePoint_Instance : public CVIBuffer_Instancing
{
private:
	CTrail_OnePoint_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_OnePoint_Instance(const CTrail_OnePoint_Instance& Prototype);
	virtual ~CTrail_OnePoint_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient);
	virtual HRESULT Initialize(void* pArg) override;

	_bool Update_Buffer(_fvector vWorldPos, _float fTimeDelta);
	_bool Spread_Buffer(_fvector vWorldPos, _float fTimeDelta);
	void Set_LifeTime(_float fTime);

public:
	virtual void Reset();

private:
	list<_Vec3> m_TrailPoses;
	_bool m_bFirst = { false };

private:
	HRESULT Ready_Buffers(const CVIBuffer_Instancing::INSTANCE_DESC& Desc);

public:
	static CTrail_OnePoint_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
END
