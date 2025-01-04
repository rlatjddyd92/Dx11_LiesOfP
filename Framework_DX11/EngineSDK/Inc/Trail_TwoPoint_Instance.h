#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CTrail_TwoPoint_Instance final : public CVIBuffer_Instancing
{
private:
	CTrail_TwoPoint_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_TwoPoint_Instance(const CTrail_TwoPoint_Instance& Prototype);
	virtual ~CTrail_TwoPoint_Instance() = default;

public:
	const list<TWOPOINT>& Get_TrailPoses() {
		return m_TrailPoses;
	}

public:
	virtual HRESULT Initialize_Prototype(const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient);
	virtual HRESULT Initialize(void* pArg) override;

	_bool Update_Buffer(_fvector vWorldTopPos, _fvector vWorldBottomPos, _float fTimeDelta);
	void Move_Buffer(_Vec3 vMoveDir, _float fSpeed, _float fTimeDelta);
	void Reset();

public:
	TWOPOINT Get_PointPos(_uint iIndex);

private:
	list<TWOPOINT> m_TrailPoses;
	_bool m_bFirst = { false };

private:
	HRESULT Ready_Buffers(const CVIBuffer_Instancing::INSTANCE_DESC& Desc);

public:
	static CTrail_TwoPoint_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const CVIBuffer_Instancing::INSTANCE_DESC& Desc, _bool isClient);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END