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
	enum COLLIDERTYPE { CT_UPPERBODY, CT_LOWERBODY, CT_LEG_LEFT, CT_LEG_RIGHT, CT_END };
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

	enum EFFECT_TYPE
	{
		P1_TRAIL, P1_STAMP, P1_CHARGESTAMP,
		EFFECT_END
	};
	//enum EFFECT_TYPE
	//{
	//	P1_TRAIL, P1_CHARGE, P1_STAMP, P2_MAGICMISSILE, P2_DARKBALL, P2_LIGHTNINGBALL, P2_LIGHTNINGSPEAR,
	//	P2_WAVETRAIL, P2_WAVE, P2_THUNDERCALLING, P2_SUMMONHAND,
	//
	//	EFFECT_END
	//};

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

	void	ChangePhase();

	virtual void	Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex = 0) override;
	virtual void	DeActive_CurretnWeaponCollider(_uint iCollIndex = 0) override;

	virtual void		Active_Effect(const _uint eType) override;
	virtual void		DeActive_Effect(const _uint eType) override;

	virtual _bool		Get_EffectsLoop(const _uint eType) override;
	virtual void	Reset_WeaponOverlapCheck(_uint iCollIndex = 0) override;

	virtual void	Change_WeaponAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration = 0.2f, _int iStartFrame = 0, _bool bEitherChange = true, _bool bSameChange = false);
	virtual _bool	Get_WeaponAnimEnd(_int iAnimIndex) override;

	virtual const _Matrix* Get_WeaponBoneCombinedMat(_uint iBoneIndex) override;
	virtual const _Matrix* Get_WeaponWorldMat() override;

private:
	vector<CGameObject*>	CollObjRenderP{};
	vector<class CEffect_Container*> m_Effects;

	class CWeapon*			m_pWeapon = { nullptr };
	class CFsm*				m_pExtraFsmCom = { nullptr };	//2페이즈 fsm
	class CModel*			m_pExtraModelCom = { nullptr };	//2페이즈 model
	
	class CCollider*		m_EXCollider[COLLTYPE_END] = { nullptr, nullptr };

	_bool					m_isChanged = { false };

	const _Matrix*			m_pColliderBindMatrix[CT_END] = { nullptr, nullptr, nullptr};


private:

	HRESULT Ready_Components();
	HRESULT Ready_FSM();
	HRESULT Ready_Weapon();
	HRESULT Ready_Effects();

	void	Update_Collider();


public:
	static CSimonManus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END