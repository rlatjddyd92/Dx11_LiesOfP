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

class CCarcassTail final : public CMonster
{

public:
	enum COLLIDERTYPE { TYPE_LEFTHAND, TYPE_RIGHTHAND, TYPE_TAIL, TYPE_IMPACT, TYPE_TAIL_2, TYPE_END };

public:
	enum CARCASS_BIGA_STATE {
		WHEELWIND = MONSTER_STATE_END,
		HEADING, LEAP, MULTYHITTINGDOWN, SCRATCHING, TAILSWINGDOWN, TAILSWIP, TAILSWIPMULTIPLE,
		HEADINGMULTIPLE, LEAPATTACK, LEAPTOATTACK, SCRATCHINGMULTIPLE, SCRATCHINGTOWIP
	};

	enum COLLIDERTYPE_BODY {
		CT_BODY_LOWER,CT_BODY_MIDDLE, CT_HEAD
		, CT_ARM_LEFT, CT_ARM_RIGHT
		, CT_LEG_LEFT, CT_LEG_RIGHT
		, CT_BODY_UPPER, CT_END
	};

private:
	CCarcassTail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCarcassTail(const CCarcassTail& Prototype);
	virtual ~CCarcassTail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void		Resetting() override;

private:
	CColliderObject* m_pColliderObject[TYPE_END] = { nullptr, nullptr, nullptr };
	class CCollider* m_EXCollider[CT_END - 1] = { nullptr, nullptr };

	const _Matrix*		m_pColliderBindMatrix[CT_END] = { nullptr, nullptr, nullptr };

private:
	virtual void	Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex = 0, HIT_TYPE eHitType = HIT_END, ATTACK_STRENGTH eAtkStrength = ATK_END) override;
	virtual void	DeActive_CurretnWeaponCollider(_uint iCollIndex = 0);

	virtual _bool		Get_EffectsLoop(const _uint eType) { return false; };

private:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();

	void	Update_Collider();

public:
	static CCarcassTail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};

END