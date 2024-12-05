#pragma once

#include "AttackObject.h"
#include "Effect_Container.h"

class CAObj_ThunderCalling	final : public CAttackObject
{
protected:
	CAObj_ThunderCalling(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAObj_ThunderCalling(const CGameObject& Prototype);
	virtual ~CAObj_ThunderCalling() = default;

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

	CTransform* m_pCopyPlayerTransformCom = { nullptr };

	_int			m_iThunderCnt = {};
	_float			m_iThunderTime = {};

	_bool			m_bEffectAlive = { true };
	_float			m_fEffectAliveTime = {};

private:
	HRESULT Ready_Components();

public:
	static CAObj_ThunderCalling* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

