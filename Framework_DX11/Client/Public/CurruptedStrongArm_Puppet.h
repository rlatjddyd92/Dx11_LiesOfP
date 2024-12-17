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

class CCurruptedStrongArm_Puppet final : public CMonster
{
public:
	enum COLLIDERTYPE { TYPE_ARM_LEFT, TYPE_ARM_RIGHT
		, TYPE_TENTACLE_FL, TYPE_TENTACLE_FR,/* TYPE_TENTACLE_BL, TYPE_TENTACLE_BR, */ TYPE_END };

	enum COLLIDERTYPE_BODY {
		CT_BODY_LOWER, CT_ARM_LEFT, CT_ARM_RIGHT, /*CT_LEG_LEFT, CT_LEG_RIGHT,*/ CT_BODY_UPPER
		, CT_END
	};

public:
	enum CARCASS_BIGA_STATE {
		SWIPATTACK = MONSTER_STATE_END,
		STINGTWICE, JUMP_PUNCH
	};

private:
	CCurruptedStrongArm_Puppet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCurruptedStrongArm_Puppet(const CCurruptedStrongArm_Puppet& Prototype);
	virtual ~CCurruptedStrongArm_Puppet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CColliderObject* m_pColliderObject[TYPE_END] = { nullptr };
	class CCollider* m_EXCollider[CT_END - 1] = { nullptr, nullptr };

private:
	virtual void	Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex = 0, _uint iHitType = 0, _uint iAtkStrength = 0) override;
	virtual void	DeActive_CurretnWeaponCollider(_uint iCollIndex = 0) override;

	virtual _bool		Get_EffectsLoop(const _uint eType) { return false; };

	const _Matrix* m_pColliderBindMatrix[CT_END] = { nullptr, nullptr, nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();

	void	Update_Collider();

public:
	static CCurruptedStrongArm_Puppet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};

END