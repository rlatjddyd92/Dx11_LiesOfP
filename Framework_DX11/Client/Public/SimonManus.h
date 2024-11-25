#pragma once

#include "Client_Defines.h"
#include "Monster.h"
#include"ColliderObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
class CFSM;
END

BEGIN(Client)

class CSimonManus final : public CMonster
{
public:
	typedef struct
	{
		_bool* pIsEndAnim { nullptr };
		_bool* pIsResetRootMove { nullptr };
		_bool* pColliderCtrs{ nullptr };
		_bool* pRootMoveCtrs{ nullptr };
		
		_double* pGrogyTrackPos { nullptr };
	}FSMSTATE_DESC;

public:
	enum COLLIDERTYPE { TYPE_LEFTHAND, TYPE_RIGHTHAND, TYPE_END };
	enum EXCOLLIDER { LEG_LEFT, LEG_RIGHT, LOWERBODY, COLLTYPE_END };

public:
	enum SIMONMANUS_P1_STATE {
		ATK_AVOIDSWING = MONSTER_STATE_END, ATK_CHASINGSWING, ATK_JUMPTOSWING, ATK_STAMP
		, ATK_STING, ATK_HIGHJUMPFALL, ATK_SWINGDOWN_L, ATK_SWINGDOWN_R
		, ATK_SWIPMULT_L, ATK_SWIPMULT_R, ATK_SWINGMULTIPLE, ATK_CHARGE_SWINGDOWN
	};

	enum SIMONMANUS_P2_STATE {
		ATKP2_AVOIDSWING = MONSTER_STATE_END, ATKP2_STING, ATKP2_SUMMONHAND, ATKP2_THUNDERBALL
		, ATKP2_THUNDERCALLING, ATKP2_WAVE, ATKP2_HIGHJUMPFALL, ATKP2_SPREADMAGIC
		, ATKP2_BRUTALATTACK, ATKP2_CHASINGSWING, ATKP2_JUMPTOATTACK, ATKP2_STAMP
		, ATKP2_LIGHTNINGTOWAVE, ATKP2_SWINGDOWN_SWING, ATKP2_SWIPMULTIPLE
		, ATKP2_ROUTE_0, ATKP2_ROUTE_1, ATKP2_ROUTE_2
	};

private:
	CSimonManus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSimonManus(const CSimonManus& Prototype);
	virtual ~CSimonManus() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	
private:
	CGameObject*			m_pColliderObject[TYPE_END] = { nullptr, nullptr };
	_bool					m_bColliderCtrs[TYPE_END] = {true,  true};
	vector<CGameObject*>	CollObjRenderP{};

	class CWeapon*	m_pWeapon = { nullptr };
	class CFsm*		m_pExtraFsmCom = { nullptr };	//2페이즈 fsm
	class CModel*	m_pExtraModelCom = { nullptr };	//2페이즈 model
	
	class CCollider* m_EXCollider[COLLTYPE_END] = { nullptr, nullptr };

	_bool			m_isChanged = { false };

private:
	virtual void	Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex = 0);
	virtual void	DeActive_CurretnWeaponCollider(_uint iCollIndex = 0);

	HRESULT Ready_Components();
	HRESULT Ready_FSM();
	HRESULT Ready_Weapon();

	void	ChangePhase();


public:
	static CSimonManus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END