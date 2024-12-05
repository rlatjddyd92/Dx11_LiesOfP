#pragma once

#include "AttackObject.h"
#include "Effect_Container.h"

class CAObj_LightningSpear	final : public CAttackObject
{
protected:
	CAObj_LightningSpear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAObj_LightningSpear(const CGameObject& Prototype);
	virtual ~CAObj_LightningSpear() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

private:
	class CEffect_Container* m_pEffect = { nullptr };

	_Vec3	m_vMoveDir{};
	_float	m_fSpeed{};

private:
	HRESULT Ready_Components();

public:
	static CAObj_LightningSpear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

