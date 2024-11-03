#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct
	{
		_float		fSpeedPerSec{}, fRotationPerSec{};
	}TRANSFORM_DESC;

	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTransform() = default;

public:
	_vector		Get_State(STATE eState) { return XMLoadFloat4x4(&m_WorldMatrix).r[eState]; }
	void		Set_State(STATE eState, _fvector vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[eState][0], vState); }

	_matrix		Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }	//연산용
	_float4x4	Get_World4x4() const { return m_WorldMatrix; }	//저장용

	_matrix		Get_WorldMatrix_Inverse() const { return XMMatrixInverse(nullptr, Get_WorldMatrix()); }


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	_float3		Get_Scaled() const;
	void		Set_Scaled(_float fX, _float fY, _float fZ);

	void		Go_Straight(_float fTimeDelta);
	void		Go_Backward(_float fTimeDelta);
	void		Go_Right(_float fTimeDelta);
	void		Go_Left(_float fTimeDelta);

	void		Turn(const _vector& vAxis, _float fTimeDelta);
	void		Rotation(const _vector& vAxis, _float fRadian);

	void		LookAt(_fvector vAt);

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

private:
	_float			m_fSpeedPerSec{};
	_float			m_fRotationPerSec{};

	_float4x4		m_WorldMatrix{};

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END