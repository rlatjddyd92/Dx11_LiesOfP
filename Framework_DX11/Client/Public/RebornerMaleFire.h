#pragma once

#include "Client_Defines.h"
#include "Monster.h"
#include "ColliderObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CFSM;
class CModel;
END

BEGIN(Client)

class CRebornerMaleFire final : public CMonster
{
public:
	enum COLLIDERTYPE_BODY {
		CT_BODY_LOWER
		, CT_UPPERARM_LEFT, CT_UPPERARM_RIGHT, CT_LOWERARM_LEFT, CT_LOWERARM_RIGHT
		, CT_UPPERLEG_LEFT, CT_UPPERLEG_RIGHT, CT_LOWERLEG_LEFT, CT_LOWERLEG_RIGHT
		, CT_BODY_UPPER, CT_END
	};

public:
	enum REBORNDERBIGA_STATE {
		BREATH = MONSTER_STATE_END
	};

private:
	CRebornerMaleFire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRebornerMaleFire(const CRebornerMaleFire& Prototype);
	virtual ~CRebornerMaleFire() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CCollider* m_EXCollider[CT_END - 1] = { nullptr, nullptr };

private:
	virtual void	Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex = 0, HIT_TYPE eHitType = HIT_END, ATTACK_STRENGTH eAtkStrength = ATK_END) override;
	virtual void	DeActive_CurretnWeaponCollider(_uint iCollIndex = 0) override;

	virtual void		Active_Effect(const _uint eType, _bool isLoop = true) override;
	virtual void		DeActive_Effect(const _uint eType) override;


	virtual _bool		Get_EffectsLoop(const _uint eType) { return false; };

	const _Matrix* m_pColliderBindMatrix[CT_END] = { nullptr, nullptr, nullptr };

	class CWeapon* m_pWeapon = { nullptr };

	class CEffect_Container* m_pFireEffect = { nullptr };
private:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();
	HRESULT Ready_Weapon();
	HRESULT Ready_Effect();

	void	Update_Collider();

public:
	static CRebornerMaleFire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};

END