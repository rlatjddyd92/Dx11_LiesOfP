#pragma once
#include "AttackObject.h"
#include "Effect_Container.h"

class CAObj_ThunderMark	final : public CAttackObject
{
protected:
	CAObj_ThunderMark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAObj_ThunderMark(const CGameObject& Prototype);
	virtual ~CAObj_ThunderMark() = default;

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
	class CEffect_Container* m_pEffectExp = { nullptr };

	_bool		m_bExplosive = { false };

private:
	HRESULT Ready_Components();

public:
	static CAObj_ThunderMark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

