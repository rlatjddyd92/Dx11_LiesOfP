#pragma once

#include "AttackObject.h"
#include "Effect_Container.h"

class CAObj_BlackBall	final : public CAttackObject
{
private:
	enum ACTIVESTATE { STATE_START, STATE_MIDDLE, STATE_LAST, STATE_END };

protected:
	CAObj_BlackBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAObj_BlackBall(const CGameObject& Prototype);
	virtual ~CAObj_BlackBall() = default;

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
	class CEffect_Container* m_pEffects[STATE_END]{ nullptr };

	CTransform* m_pCopyPlayerTransformCom = { nullptr };

	_bool			m_bShoted = { false };
	_bool			m_bEnd = { false };

	_bool			m_bSoundControl = { false };

	_int	m_iThrowPhase{};

	_Vec3	m_vMoveDir{};
	_float	m_fSpeed{};

	_float	m_fThrowTime{};
	_float	m_fDelayTime{};

private:
	HRESULT Ready_Components();

public:
	static CAObj_BlackBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

