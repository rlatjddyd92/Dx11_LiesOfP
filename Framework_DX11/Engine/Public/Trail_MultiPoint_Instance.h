#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)
class ENGINE_DLL CTrail_MultiPoint_Instance final : public CVIBuffer_Instancing
{
public:
	typedef struct
	{
		PARTICLE Particle;
		_float4 vPreTranslation;
		_float4 vCurrentRandomPos;
		_float4 vNextRandomPos;
		_uint iTailInitIndex;
	}HEAD_PARTICLE;

	typedef struct
	{
		PARTICLE Particle;
		_float4 vMoveDir;
		_float4 vPreTranslation;
	}TAIL_PARTICLE;

	typedef struct : CVIBuffer_Instancing::INSTANCE_DESC
	{
		_uint		iTail_NumInstance;
		_float2		vTail_Speed;
		_float2		vTail_Size;
		_float2		vTail_LifeTime;
		_float4		vTail_MinColor;
		_float4		vTail_MaxColor;
	}TRAIL_MP_INSTANCE_DESC;

	typedef struct
	{
		_uint		iNumInstance;
		_uint		iNumTailInstance;
		_uint		iState;
		_float		fGravity;

		_float4		vPivot;

		_float4		vMoveDir;

		_float4x4	WorldMatrix;

		_float3		vOrbitAxis;
		_float		fOrbitAngle;

		_float		fTimeInterval;
		_float		fRandomRatio;
		_float		fAccelSpeed;
		_float		fAccelLimit;

		_float		fTimeDelta;
		_float		fTailInterval;    
		_float		fTailDistance;
		_float		fPadding;

		_float4		vTailMoveDir;
	}TRAIL_MP_MOVEMENT;

private:
	CTrail_MultiPoint_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_MultiPoint_Instance(const CTrail_MultiPoint_Instance& Prototype);
	virtual ~CTrail_MultiPoint_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const TRAIL_MP_INSTANCE_DESC& Desc, _bool isClient);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_HeadBuffer(class CShader_NonVTX* pShader, const _char* pConstantName);
	virtual HRESULT Bind_TailBuffer(class CShader_NonVTX* pShader, const _char* pConstantName);
	virtual HRESULT Render_Head();
	virtual HRESULT Render_Tail();


public:
	_bool DispatchCS(class CShader_Compute* pComputeShader, const TRAIL_MP_MOVEMENT& MovementData);

public:
	virtual void Reset();

private:
	_bool		m_bFirst = { false };
	_float		m_fTime = { 0.f };
	_uint		m_iNumTailInstance	= { 0 };
	_float2		m_vTailSpeed		= {};
	_float2		m_vTail_Size		= {};
	_float2		m_vTail_LifeTime	= {};
	_float4		m_vTail_MinColor	= {};
	_float4		m_vTail_MaxColor	= {};

	D3D11_BUFFER_DESC					m_HeadBuffer_Desc = {};
	D3D11_BUFFER_DESC					m_TailBuffer_Desc = {};
	D3D11_BUFFER_DESC					m_MoveBuffer_Desc = {};	// 움직임용 버퍼 하나로 통일.

	D3D11_UNORDERED_ACCESS_VIEW_DESC	m_HeadUAV_Desc = {};
	D3D11_SHADER_RESOURCE_VIEW_DESC		m_HeadSRV_Desc = {};

	D3D11_UNORDERED_ACCESS_VIEW_DESC	m_TailUAV_Desc = {};
	D3D11_SHADER_RESOURCE_VIEW_DESC		m_TailSRV_Desc = {};

	ID3D11Buffer*						m_pHeadBuffer = { nullptr };
	ID3D11UnorderedAccessView*			m_pHeadUAV = { nullptr };
	ID3D11ShaderResourceView*			m_pHeadSRV = { nullptr };

	ID3D11Buffer*						m_pTailBuffer = { nullptr };
	ID3D11UnorderedAccessView*			m_pTailUAV = { nullptr };
	ID3D11ShaderResourceView*			m_pTailSRV = { nullptr };

	ID3D11Buffer*						m_pMoveBuffer = { nullptr };

	D3D11_BUFFER_DESC					m_InitHeadBuffer_Desc = {};
	D3D11_SHADER_RESOURCE_VIEW_DESC		m_InitHeadSRV_Desc = {};
	ID3D11Buffer*						m_pInitHeadBuffer = { nullptr };
	ID3D11ShaderResourceView*			m_pInitHeadSRV = { nullptr };

	D3D11_BUFFER_DESC					m_InitTailBuffer_Desc = {};
	D3D11_SHADER_RESOURCE_VIEW_DESC		m_InitTailSRV_Desc = {};
	ID3D11Buffer*						m_pInitTailBuffer = { nullptr };
	ID3D11ShaderResourceView*			m_pInitTailSRV = { nullptr };


	void* m_pInstanceTailes = { nullptr };
	D3D11_SUBRESOURCE_DATA		m_TailInitialData = {};

private:
	HRESULT Ready_Buffers(const TRAIL_MP_INSTANCE_DESC& Desc);
	HRESULT Init_HeadParticle(HEAD_PARTICLE* pParticles);
	HRESULT Init_TailParticle(TAIL_PARTICLE* pParticles);

public:
	static CTrail_MultiPoint_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TRAIL_MP_INSTANCE_DESC& Desc, _bool isClient);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};
END
