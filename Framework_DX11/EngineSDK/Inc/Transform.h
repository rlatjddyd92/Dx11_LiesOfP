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
	_Vec4				Get_State(STATE eState) { return XMLoadFloat4x4(&m_WorldMatrix).r[eState]; }
	void				Set_State(STATE eState, _Vec3 vState) { XMStoreFloat3((_float3*)&m_WorldMatrix.m[eState][0], vState); }

	_Matrix				Get_WorldMatrix() const { return XMLoadFloat4x4(&m_WorldMatrix); }	//연산용
	void				Set_WorldMatrix(const _Matrix WorldMatrix) { XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix); }
	const _Matrix*		Get_WorldMatrix_Ptr() { return &m_WorldMatrix; }	//저장용

	_Matrix				Get_WorldMatrix_Inverse() const { return XMMatrixInverse(nullptr, Get_WorldMatrix()); }

	_Vec3				Get_CurrentRotation() { return _Vec3(m_fCurrentRotationX, m_fCurrentRotationY, m_fCurrentRotationZ); }
	void				Set_CurrentRotation(_Vec3 vRot) { m_fCurrentRotationX = vRot.x; m_fCurrentRotationY = vRot.y; m_fCurrentRotationZ = vRot.z; }

public:	
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	
public:
	_Vec3		Get_Scaled() const;
	void		Set_Scaled(_float fX, _float fY, _float fZ);

	void		LookAt(_Vec4 vAt);
	void		LookAt_NoHeight(_Vec4 vAt);
	void		LookAt_Lerp(_Vec4 vAt, _float fSpeed, _float fTimeDelta);
	void		LookAt_Lerp_NoHeight(_Vec4 vAt, _float fSpeed, _float fTimeDelta);
	void		LookAt_Dir(_Vec4 vDir);

	void		Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void		Go_Backward(_float fTimeDelta);
	void		Go_Left(_float fTimeDelta);
	void		Go_Right(_float fTimeDelta);

	void		Turn(_Vec4 vAxis, _float fTimeDelta, _float fSpeed = 0.f);
	void		Turn(_bool isRotationX, _bool isRotationY, _bool isRotationZ, _float fTimeDelta, _float fSpeed = 0.f);

	void		Rotation(const _Vec4& vAxis, _float fRadian);
	void		Rotation(_float fX, _float fY, _float fZ);

	void		BillBoard();
	void		BillBoard_NoHeight();

	void		Set_NewUp(_Vec3 vNewUp);
	void		Set_NewLook(_Vec3 vNewLook);

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);


private:
	_float			m_fSpeedPerSec{};
	_float			m_fRotationPerSec{};

	_Matrix			m_WorldMatrix = {};

	_float			m_fCurrentRotationX = { 0.f };
	_float			m_fCurrentRotationY = { 0.f };
	_float			m_fCurrentRotationZ = { 0.f };

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END