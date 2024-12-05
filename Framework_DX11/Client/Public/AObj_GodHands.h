#pragma once

#include "AttackObject.h"
#include "Effect_Container.h"

class CAObj_GodHands	final : public CAttackObject
{
private:
	enum ACTIVESTATE { STATE_SIGN, STATE_DROP, STATE_BLAST, STATE_END };

protected:
	CAObj_GodHands(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAObj_GodHands(const CGameObject& Prototype);
	virtual ~CAObj_GodHands() = default;

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
	class CEffect_Container* m_pEffects[STATE_END] = {nullptr};

	_bool			m_bSummoned = { false };
	_bool			m_bEnd = { false };
	_bool			m_bAttack = { false };

	_int	m_iDropPhase{};

	_Vec3	m_vMoveDir{};
	_Vec3	m_vOriginPos{};
	_float	m_fSpeed{};

	_float	m_fHandActTime{};


private:
	HRESULT Ready_Components();

public:
	static CAObj_GodHands* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

