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
	enum COLLIDERTYPE { TYPE_LEFTHAND, TYPE_RIGHTHAND, TYPE_TAIL, TYPE_IMPACT, TYPE_END };

public:
	enum CARCASS_BIGA_STATE {
		WHEELWIND = MONSTER_STATE_END,
		HEADING, LEAP, MULTYHITTINGDOWN, SCRATCHING, TAILSWINGDOWN, TAILSWIP, TAILSWIPMULTIPLE,
		HEADINGMULTIPLE, LEAPATTACK, LEAPTOATTACK, SCRATCHINGMULTIPLE, SCRATCHINGTOWIP
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

private:
	CColliderObject* m_pColliderObject[TYPE_END] = { nullptr, nullptr, nullptr };
	_bool					m_bColliderCtrs[TYPE_END] = { true,  true, true };

private:
	virtual void	Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex = 0, _uint iHitType = 0, _uint iAtkStrength = 0);
	virtual void	DeActive_CurretnWeaponCollider(_uint iCollIndex = 0);

	virtual _bool		Get_EffectsLoop(const _uint eType) { return false; };

private:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();


public:
	static CCarcassTail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};

END