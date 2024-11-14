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
	virtual HRESULT Initialize_Prototype(const TRAIL_MP_INSTANCE_DESC& Desc, _bool isClient);
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Reset();

private:
	_bool m_bFirst = { false };
	_float m_fTime = { 0.f };
	_uint m_iNumTailInstance = { 0 };

	D3D11_BUFFER_DESC					m_HeadBuffer_Desc = {};
	D3D11_BUFFER_DESC					m_TailBuffer_Desc = {};
	D3D11_BUFFER_DESC					m_MoveBuffer_Desc = {};	// 움직임용 버퍼 하나로 통일.

	D3D11_UNORDERED_ACCESS_VIEW_DESC	m_HeadUAV_Desc = {};

	D3D11_UNORDERED_ACCESS_VIEW_DESC	m_TailUAV_Desc = {};
	D3D11_SHADER_RESOURCE_VIEW_DESC		m_TailSRV_Desc = {};

	ID3D11Buffer*						m_pHeadBuffer = { nullptr };
	ID3D11UnorderedAccessView*			m_pHeadUAV = { nullptr };

	ID3D11Buffer*						m_pTailBuffer = { nullptr };
	ID3D11UnorderedAccessView*			m_pTailUAV = { nullptr };
	ID3D11ShaderResourceView*			m_pTailSRV = { nullptr };

	ID3D11Buffer*						m_pHeadMoveBuffer = { nullptr };
	ID3D11Buffer*						m_pTailMoveBuffer = { nullptr };

	D3D11_BUFFER_DESC					m_InitHeadBuffer_Desc = {};
	D3D11_SHADER_RESOURCE_VIEW_DESC		m_InitHeadSRV_Desc = {};
	ID3D11Buffer*						m_pInitHeadBuffer = { nullptr };
	ID3D11ShaderResourceView*			m_pInitHeadSRV = { nullptr };

private:
	HRESULT Ready_Buffers(const TRAIL_MP_INSTANCE_DESC& Desc);

public:
	static CTrail_MultiPoint_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TRAIL_MP_INSTANCE_DESC& Desc, _bool isClient);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};
END
