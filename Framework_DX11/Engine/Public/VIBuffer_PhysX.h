#pragma once

#include "VIBuffer.h"

BEGIN(Engine)


class CVIBuffer_PhysX : public CVIBuffer
{
public:
	typedef struct
	{
		_float3 _vSize{};
		_float3 vOffSetPosition = { 0.0f,0.0f,0.0f };
		_Quaternion vOffSetQuat = { 0.0f,0.0f,0.0f,1.0f };
	}VIBUFFER_PHYSX_DESC;

public:
	CVIBuffer_PhysX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVIBuffer_PhysX() = default;

private:
	HRESULT Initialize(VIBUFFER_PHYSX_DESC* pDesc);

public:
	static CVIBuffer_PhysX* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, VIBUFFER_PHYSX_DESC* pDesc);
	virtual CComponent* Clone(void* pArg) {};
	virtual void Free() override;

};

END