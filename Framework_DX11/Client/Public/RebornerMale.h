#pragma once

#include "Client_Defines.h"
#include "Monster.h"
#include "ColliderObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CFSM;
END

BEGIN(Client)

class CRebornerMale final : public CMonster
{

public:
	enum COLLIDERTYPE { TYPE_LEFTHAND, TYPE_RIGHTHAND, TYPE_IMPACT, TYPE_END };

public:
	enum REBORNER_MALE_STATE {
		WHEELWIND = MONSTER_STATE_END,
	};

private:
	CRebornerMale(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRebornerMale(const CRebornerMale& Prototype);
	virtual ~CRebornerMale() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CColliderObject* m_pColliderObject[TYPE_END] = { nullptr, nullptr, nullptr };
	_bool					m_bColliderCtrs[TYPE_END] = { true,  true, true };

private:
	virtual void	Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex = 0, _uint iHitType = 0, _uint iAtkStrength = 0) override;
	virtual void	DeActive_CurretnWeaponCollider(_uint iCollIndex = 0) override;

	virtual _bool		Get_EffectsLoop(const _uint eType) { return false; };

private:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();


public:
	static CRebornerMale* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};

END