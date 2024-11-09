#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
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

private:
	_bool m_bFirst = { false };
	vector<_float3> m_CurrentRandomDir;
	vector<_float3> m_NextRandomDir;

private:
	void Set_NextRandomDir();
	_Vec4 Get_RandomDir(_uint iIndex, _float fTime);
	_Vec4 Get_RevolveDir(_Vec4 vTargetDir, _Vec3 vRevolveAxis, _float fAngle, _float fTimeDelta);

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END