#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)
class ENGINE_DLL CTrail_MultiPoint_Instance final : public CVIBuffer_Instancing
{
public:
	typedef struct : CVIBuffer_Instancing::INSTANCE_DESC
	{
		_uint iNumTailInstance;
	}TRAIL_MP_INSTANCE_DESC;

private:
	CTrail_MultiPoint_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_MultiPoint_Instance(const CTrail_MultiPoint_Instance& Prototype);
	virtual ~CTrail_MultiPoint_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC& Desc, _bool isClient);
	virtual HRESULT Initialize(void* pArg) override;

	HRESULT Initialize_Desc(const CVIBuffer_Instancing::INSTANCE_DESC& Desc);

private:
	_bool m_bFirst = { false };
	_float m_fTime = { 0.f };

	D3D11_BUFFER_DESC					m_HeadBuffer_Desc = {};
	D3D11_BUFFER_DESC					m_HeadMoveBuffer_Desc = {};

	D3D11_BUFFER_DESC					m_TailBuffer_Desc = {};
	D3D11_BUFFER_DESC					m_TailMoveBuffer_Desc = {};

	D3D11_UNORDERED_ACCESS_VIEW_DESC	m_HeadUAV_Desc = {};
	D3D11_SHADER_RESOURCE_VIEW_DESC		m_TailUAV_Desc = {};

	ID3D11Buffer* m_pHeadBuffer = { nullptr };
	ID3D11UnorderedAccessView* m_pHeadUAV = { nullptr };

	ID3D11Buffer* m_pTailBuffer = { nullptr };
	ID3D11UnorderedAccessView* m_pTailUAV = { nullptr };
	ID3D11ShaderResourceView* m_pTailSRV = { nullptr };

	ID3D11Buffer* m_pHeadMoveBuffer = { nullptr };
	ID3D11Buffer* m_pTailMoveBuffer = { nullptr };

	D3D11_BUFFER_DESC	m_InitHeadBuffer_Desc = {};
	ID3D11Buffer* m_pInitHeadBuffer = { nullptr };
	ID3D11ShaderResourceView* m_pInitHeadSRV = { nullptr };

public:
	virtual void Reset();

public:
	static CTrail_MultiPoint_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC& Desc, _bool isClient);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};
END
