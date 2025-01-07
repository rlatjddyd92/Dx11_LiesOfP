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

class CCarcassNormal final : public CMonster
{
public:
	enum COLLIDERTYPE {
		TYPE_HAND_LEFT, TYPE_HAND_RIGHT, TYPE_HEAD, TYPE_END
	};

	enum COLLIDERTYPE_BODY {
		CT_BODY_LOWER, CT_HEAD
		, CT_UPPERARM_LEFT, CT_UPPERARM_RIGHT, CT_LOWERARM_LEFT, CT_LOWERARM_RIGHT
		, CT_UPPERLEG_LEFT, CT_UPPERLEG_RIGHT, CT_LOWERLEG_LEFT, CT_LOWERLEG_RIGHT
		, CT_BODY_UPPER, CT_END
	};

public:
	enum CARCASS_NORMAL_STATE {
		TRIPLECLAW = MONSTER_STATE_END,
		TRIPLECLAW_2, CLAWMULTIPLE, CLAWRUSH, BITE, HEADINGMULTIPLE
	};

private:
	CCarcassNormal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCarcassNormal(const CCarcassNormal& Prototype);
	virtual ~CCarcassNormal() = default;

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
	CColliderObject* m_pColliderObject[TYPE_END] = { nullptr };
	class CCollider* m_EXCollider[CT_END - 1] = { nullptr, nullptr };

private:
	virtual void	Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex = 0, HIT_TYPE eHitType = HIT_END, ATTACK_STRENGTH eAtkStrength = ATK_END) override;
	virtual void	DeActive_CurrentWeaponCollider(_uint iCollIndex = 0) override;

	virtual void		Active_Debuff(_int iIndex, _float fDebuffRatio = 1.f);
	virtual void		DeActive_Debuff(_int iIndex);

	virtual _bool		Get_EffectsLoop(const _uint eType) { return false; };

	const _Matrix* m_pColliderBindMatrix[CT_END] = { nullptr, nullptr, nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();
	HRESULT Ready_Weapon();
	HRESULT Ready_Effect();

	void	Update_Collider();

public:
	static CCarcassNormal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};

END