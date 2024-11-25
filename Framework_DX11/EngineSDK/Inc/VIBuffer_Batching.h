#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Batching final : public CVIBuffer
{
private:
	CVIBuffer_Batching(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Batching(const CVIBuffer_Batching& Prototype);
	virtual ~CVIBuffer_Batching() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	static CVIBuffer_Batching* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END