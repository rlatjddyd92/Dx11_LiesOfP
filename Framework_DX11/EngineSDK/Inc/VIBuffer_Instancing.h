#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	typedef struct
	{
		_uint		iNumInstance;
		_float3		vCenter;
		_float3		vRange;
		_float3		vExceptRange;
		_float2		vSize;
		_float2		vSpeed;
		_float2		vLifeTime;
		_float4		vMinColor;
		_float4		vMaxColor;
	}INSTANCE_DESC;

	typedef struct
	{
		_uint		iNumInstance;
		_uint		iState;
		_float2		vPadding_1;

		_float4		vPivot;

		_float		fGravity;
		_float3		vPadding_2;

		_float4		vMoveDir;

		_float4x4	WorldMatrix;

		_float3		vOrbitAxis;
		_float		fOrbitAngle;

		_float		fTimeInterval;
		_float		fRandomRatio;
		_float		fAccelSpeed;
		_float		fAccelLimit;

		_float		fTimeDelta;
		_float3		vPadding_3;
	}PARTICLE_MOVEMENT;

	enum STATE
	{
		STATE_ORBIT				= 0x0001,
		STATE_RANDOM			= 0x0002,
		STATE_LOOP				= 0x0004,
		STATE_ACCEL				= 0x0008,
		STATE_DECEL				= 0x0010,
		STATE_TAILSPRAED		= 0x0020,
		STATE_TRAIL				= 0x0040,
		STATE_END				= 0x9999
	};

	enum TYPE
	{
		TYPE_SPREAD,
		TYPE_MOVE,
		TYPE_CONVERGE,
		TYPE_END
	};

protected:
	CVIBuffer_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& Prototype);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC& Desc, _bool isClient) ;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffers() override;
	virtual HRESULT Render() override;

	HRESULT Initialize_Desc(const CVIBuffer_Instancing::INSTANCE_DESC& Desc);

public:
	virtual void Reset();

protected:
	_bool Contain_State(_uint iCurrentState, _uint iCheckState);
	void Init_Particle(PARTICLE* pParticles);
	_float4 Get_ExceptedTranslation();

	HRESULT Ready_Buffers(const INSTANCE_DESC& Desc);

protected:
	ID3D11Buffer*				m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};
	D3D11_SUBRESOURCE_DATA		m_InstanceInitialData = {};

	_uint						m_iInstanceStride = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };
	_uint						m_iNumInstance = { 0 };
	_float3						m_vCenterPos = {};
	_float3						m_vRange = {};
	_float3						m_vExceptRange = {};
	_float2						m_vSize = {};
	_float2						m_vLifeTime = {};
	_float4						m_vMinColor = {};
	_float4						m_vMaxColor = {};
	_float2						m_vSpeed = {};
	_float*						m_pSpeed = { nullptr };

	_uint						m_iNumRender = { 0 };

	void*						m_pInstanceVertices = { nullptr };

	_bool						m_isClient = { false };

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END