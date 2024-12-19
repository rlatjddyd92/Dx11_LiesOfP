#pragma once

#include "AttackObject.h"
#include "Effect_Container.h"

BEGIN(Client)
class CRaxasia;
END

class CAObj_ThunderBolt	final : public CAttackObject
{
private:
	enum ACTIVESTATE { STATE_SIGN, STATE_CREATE, STATE_NORMAL, STATE_IMPACT
		, STATE_END };

protected:
	CAObj_ThunderBolt(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAObj_ThunderBolt(const CGameObject& Prototype);
	virtual ~CAObj_ThunderBolt() = default;

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
	class CEffect_Container* m_pEffects[STATE_END] = { nullptr };
	class CRaxasia* m_pCopyRaxasia = { nullptr };

	_bool   m_bCounter = { false };
	_bool   m_bImpact = { false };

	_Vec3	m_vMoveDir{};
	_Vec3	m_vTargetPos{};

	_float	m_fLifeTime = {0.f};
	_float	m_fLifeDuration = {10.f};


	_float	m_fSpeed{};

	_float	m_fTimer{};
	_float	m_fHeightGap{};

	_int	m_iStateTrack{};

private:
	HRESULT Ready_Components();

public:
	static CAObj_ThunderBolt* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};


