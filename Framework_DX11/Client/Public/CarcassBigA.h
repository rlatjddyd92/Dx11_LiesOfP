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

class CCarcassBigA final : public CMonster
{

public:
	enum COLLIDERTYPE { TYPE_LEFTHAND, TYPE_RIGHTHAND, TYPE_IMPACT, TYPE_END };

	enum COLLIDERTYPE_BODY {
		CT_BODY_LOWER
		, CT_ARM_LEFT, CT_ARM_RIGHT
		, CT_LEG_LEFT, CT_LEG_RIGHT
		, CT_BODY_UPPER, CT_END
	};

public:
	enum CARCASS_BIGA_STATE {
		WHEELWIND = MONSTER_STATE_END,
		LT_SWINGRIGHT, LO_SWINGRIGHT, RAGE_ATTACK,
		ATK_ROUTE_1, ATK_ROUTE_2, ATK_ROUTE_3, ATK_ROUTE_4, ATK_IMPACT
	};

private:
	CCarcassBigA(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCarcassBigA(const CCarcassBigA& Prototype);
	virtual ~CCarcassBigA() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void		Resetting() override;
	virtual void		On_PowerAttack(_bool bOn) override;

private:
	CColliderObject*			m_pColliderObject[TYPE_END] = { nullptr, nullptr, nullptr };
	class CCollider*			m_EXCollider[CT_END - 1] = { nullptr, nullptr };
	const _Matrix*				m_pColliderBindMatrix[CT_END] = { nullptr, nullptr, nullptr };

	class CDissolve_PowerAttack* m_pPowerAttackEffect = { nullptr };

private:
	virtual void	Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex = 0, HIT_TYPE eHitType = HIT_END, ATTACK_STRENGTH eAtkStrength = ATK_END) override;
	virtual void	DeActive_CurrentWeaponCollider(_uint iCollIndex = 0) override;

	virtual _bool		Get_EffectsLoop(const _uint eType) { return false; };

private:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();
	HRESULT Ready_Effect();

	void	Update_Collider();

public:
	static CCarcassBigA* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};

END