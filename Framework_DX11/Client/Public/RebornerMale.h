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
	enum COLLIDERTYPE_BODY {
		CT_BODY_LOWER
		, CT_UPPERARM_LEFT, CT_UPPERARM_RIGHT, CT_LOWERARM_LEFT, CT_LOWERARM_RIGHT
		, CT_UPPERLEG_LEFT, CT_UPPERLEG_RIGHT, CT_LOWERLEG_LEFT, CT_LOWERLEG_RIGHT
		, CT_BODY_UPPER, CT_END
	};

public:
	enum REBORNDERBIGA_STATE {
		SWINGMULTIPLE = MONSTER_STATE_END
		, RUSHSTING, GUARDSTING
		, SLASHTWICE, SLASHJUMP
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
	class CCollider* m_EXCollider[CT_END - 1] = { nullptr, nullptr };

private:
	virtual void	Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex = 0, _uint iHitType = 0, _uint iAtkStrength = 0) override;
	virtual void	DeActive_CurretnWeaponCollider(_uint iCollIndex = 0) override;

	virtual _bool		Get_EffectsLoop(const _uint eType) { return false; };

	const _Matrix* m_pColliderBindMatrix[CT_END] = { nullptr, nullptr, nullptr };

	class CWeapon* m_pWeapon = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();
	HRESULT Ready_Weapon();

	void	Update_Collider();

public:
	static CRebornerMale* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};

END