#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instancing
{
public:
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


private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffer(class CShader_NonVTX* pShader, const _char* pConstantName);
	virtual HRESULT Render();

public:
	void Spread_Test(class CShader_Compute* pComputeShader, const PARTICLE_MOVEMENT& MovementData);

#pragma region KEEP
	virtual _bool Spread(_uint iState, _Matrix WorldMatrix, _float fRenderRatio, _Vec4 vPivot, _float fGravity, _float fTimeDelta,
		_Vec3 vRevolveAxis = {}, _float fAngle = { 0.f }, _float fTimeInterval = { 0.f }, _float fRandomRatio = {1.f}, 
		_float fAccelLimit = { 0.f }, _float fAccelSpeed = { 1.f });

	virtual _bool Move(_uint iState, _Matrix WorldMatrix, _float fRenderRatio, _Vec4 vDir, _float fGravity, _float fTimeDelta,
		_Vec4 vCenter = {}, _Vec3 vRevolveAxis = {}, _float fAngle = { 0.f }, _float fTimeInterval = { 0.f }, _float fRandomRatio = { 1.f }, 
		_float fAccelLimit = { 0.f }, _float fAccelSpeed = { 1.f });

	virtual _bool Converge(_uint iState, _float fRenderRatio, _Vec4 vPivot, 
		_float fTimeDelta, _Vec3 vRevolveAxis = {}, _float fAngle = { 0.f }, _float fTimeInterval = { 0.f }, _float fRandomRatio = { 1.f }, 
		_float fAccelLimit = { 0.f }, _float fAccelSpeed = { 1.f });

	virtual _bool Spread_Independent(_uint iState, _Matrix WorldMatrix, _float fRenderRatio, _Vec4 vPivot, _float fGravity, _float fTimeDelta, 
		_Vec3 vRevolveAxis = {}, _float fAngle = { 0.f }, _float fTimeInterval = { 0.f }, _float fRandomRatio = { 1.f }, 
		_float fAccelLimit = { 0.f }, _float fAccelSpeed = { 1.f });

	virtual _bool Move_Independent(_uint iState, _Matrix WorldMatrix, _float fRenderRatio, _Vec4 vDir, _float fGravity,_float fTimeDelta, 
		_Vec4 vCenter = {}, _Vec3 vRevolveAxis = {}, _float fAngle = { 0.f }, _float fTimeInterval = { 0.f }, _float fRandomRatio = { 1.f }, 
		_float fAccelLimit = { 0.f }, _float fAccelSpeed = { 1.f });

	virtual _bool Converge_Independent(_uint iState, _Matrix WorldMatrix, _float fRenderRatio, _Vec4 vPivot, _float fTimeDelta, 
		_Vec3 vRevolveAxis = {}, _float fAngle = { 0.f }, _float fTimeInterval = { 0.f }, _float fRandomRatio = { 1.f }, 
		_float fAccelLimit = { 0.f }, _float fAccelSpeed = { 1.f });

	virtual void Reset() override;
#pragma endregion

private:
	_bool m_bFirst = { false };
	vector<_float3> m_CurrentRandomDir;
	vector<_float3> m_NextRandomDir;

	D3D11_BUFFER_DESC					m_ParticleBuffer_Desc = {};
	D3D11_BUFFER_DESC					m_MoveBuffer_Desc = {};
	D3D11_UNORDERED_ACCESS_VIEW_DESC	m_UAV_Desc = {};
	D3D11_SHADER_RESOURCE_VIEW_DESC		m_SRV_Desc = {};

	ID3D11Buffer*				m_pParticleBuffer	= nullptr;
	ID3D11Buffer*				m_pMovementBuffer	= { nullptr };
	ID3D11UnorderedAccessView*	m_pParticleUAV		= nullptr;
	ID3D11ShaderResourceView*	m_pParticleSRV		= nullptr;

private:
	void Set_NextRandomDir();
	_Vec4 Get_RandomDir(_uint iIndex, _float fTime);
	_Vec4 Get_RevolveDir(_Vec4 vTargetDir, _Vec3 vRevolveAxis, _float fAngle, _float fTimeDelta);

	void Init_Particle(PARTICLE* pParticles);
	_float4 Get_RandomTranslation();

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
END