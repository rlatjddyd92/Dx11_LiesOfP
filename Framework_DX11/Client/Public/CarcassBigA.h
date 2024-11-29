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

public:
	enum CARCASS_BIGA_STATE {
		WHEELWIND = MONSTER_STATE_END,
		LT_SWINGRIGHT, LO_SWINGRIGHT, RAGE_ATTACK,
		ATK_ROUTE_0, ATK_ROUTE_1, ATK_ROUTE_2,
		ATK_ROUTE_3, ATK_ROUTE_4, ATK_IMPACT
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

private:
	CColliderObject*			m_pColliderObject[TYPE_END] = { nullptr, nullptr, nullptr };
	_bool					m_bColliderCtrs[TYPE_END] = {true,  true, true};

private:
	virtual void	Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex = 0);
	virtual void	DeActive_CurretnWeaponCollider(_uint iCollIndex = 0);

	virtual _bool		Get_EffectsLoop(const _uint eType) { return false; };

private:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();


public:
	static CCarcassBigA* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};

END