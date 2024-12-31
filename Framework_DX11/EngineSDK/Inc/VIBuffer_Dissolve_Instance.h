#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Dissolve_Instance final : public CVIBuffer_Instancing
{
public:

	typedef struct
	{
		// 모델에서 만들고 전달해야 함.
		_uint		iNumInstance;
		DISSOLVE_PARTICLE*	pParticles;
		_uint		iMeshIndex;
		_float		fMaxLifeTime;
	} DISSOLVE_INSTANCE_DESC;

	typedef struct
	{
		_float4x4			m_BoneMatrices[g_iMaxMeshBones] = {};
		_float				fThreshold = { 0 };
		_uint				iModelType = { 0 };
		_float2				vTextureSize = {};
	}DISSOLVE_INITIAL;

	typedef struct
	{
		_float				fThreshold = { 0 };
		_uint				iModelType = { 0 };
		_float2				vTextureSize = {};
	}DISSOLVE_DATA;

private:
	CVIBuffer_Dissolve_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Dissolve_Instance(const CVIBuffer_Dissolve_Instance& Prototype);
	virtual ~CVIBuffer_Dissolve_Instance() = default;

public:
	HRESULT Initialize_Prototype(const DISSOLVE_INSTANCE_DESC& Desc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Buffer(class CShader_NonVTX* pShader, const _char* pConstantName);
	virtual HRESULT Render();

	virtual void Reset() override;

public:
	_bool DispatchCS(class CShader_Compute* pComputeShader, class CTexture* pTexture, class CModel* pModel, const PARTICLE_MOVEMENT& MovementData, const DISSOLVE_DATA& DissolveData);

private:
	_bool m_bFirst = { false };
	_float m_fTime = { 0.f };

	D3D11_BUFFER_DESC					m_ParticleBuffer_Desc = {};
	D3D11_BUFFER_DESC					m_MoveBuffer_Desc = {};
	D3D11_BUFFER_DESC					m_DissolveBuffer_Desc = {};

	D3D11_UNORDERED_ACCESS_VIEW_DESC	m_UAV_Desc = {};
	D3D11_SHADER_RESOURCE_VIEW_DESC		m_SRV_Desc = {};

	ID3D11Buffer* m_pParticleBuffer = nullptr;
	ID3D11Buffer* m_pMovementBuffer = { nullptr };
	ID3D11Buffer* m_pDissolveBuffer = { nullptr };

	ID3D11UnorderedAccessView* m_pParticleUAV = nullptr;
	ID3D11ShaderResourceView* m_pParticleSRV = nullptr;

	D3D11_BUFFER_DESC	m_InitParticleBuffer_Desc = {};
	ID3D11Buffer*		m_pInitParticleBuffer = nullptr;
	ID3D11ShaderResourceView* m_pInitParticleSRV = nullptr;

	//_Matrix Test[g_iMaxMeshBones] = {};

private:
	HRESULT Ready_Buffers(const DISSOLVE_INSTANCE_DESC& Desc);
	void For_Debug();

public:
	static CVIBuffer_Dissolve_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const DISSOLVE_INSTANCE_DESC& Desc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};
END