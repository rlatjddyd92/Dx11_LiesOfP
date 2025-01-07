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
	enum CUTSCENE_MODEL { MODEL_PHASE1, MODEL_PHASE2, MODEL_END };
	enum CUTSCENE_TYPE { CUTSCENE_MEET, CUTSCENE_P2, CUTSCENE_DIE, CUTSCENE_END };

public:
	enum SIMONMANUS_P1_STATE {
		ATK_STING = MONSTER_STATE_END, ATK_CHASINGSWING, ATK_JUMPTOSWING, ATK_STAMP
		, ATK_HIGHJUMPFALL, ATK_SWINGDOWN_L, ATK_SWINGDOWN_R, ATK_SWIPMULT_L
		, ATK_SWIPMULT_R, ATK_SWINGMULTIPLE, ATK_CHARGE_SWINGDOWN
	};

	enum SIMONMANUS_P2_STATE {
		ATKP2_AVOIDSWING = MONSTER_STATE_END, ATKP2_STING, ATKP2_SUMMONHAND, ATKP2_THUNDERBALL
		, ATKP2_THUNDERCALLING, ATKP2_WAVE, ATKP2_HIGHJUMPFALL, ATKP2_SPREADMAGIC, ATKP2_SLIDEMAGIC
		, ATKP2_BRUTALATTACK, ATKP2_CHASINGSWING, ATKP2_JUMPTOATTACK, ATKP2_STAMP
		, ATKP2_BACKLIGHTNING, ATKP2_SWINGDOWN_SWING, ATKP2_SWIPMULTIPLE
		, ATKP2_ROUTE_1, ATKP2_ROUTE_2, DIE_TALKING
	};

	enum SIMONMANUS_CUTSCENE_STATE {
		STATE_MEET, STATE_P2, STATE_DIE, STATE_END
	};

	enum EFFECT_TYPE
	{
		P1_TRAIL, P1_CHARGEHAMMER,
		P2_TRAIL, SWING_DRAG, SWING_DRAG_REVERSE, P2_SLIDEMAGIC,
		P2_JUMPMAGIC, P2_SH_CHARGE, P2_SH_EXPLOSION, P2_WAVE_TRAIL,
		WEAPON_PARTICLE,

		CUTSCENE_P2_ARM_PARTICLE, CUTSCENE_P2_MAP,


		EFFECT_END
	};

	enum DISSOLVE_EFFECT
	{
		DISSOLVE_DEAD, DISSOLVE_AURA,
		DISSOLVE_POWERATTACK_P1, DISSOLVE_POWERATTACK_P2,
		DISSOLVE_END
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

	void	ChangePhase();

	virtual void	Active_Weapon() { m_pWeapon->IsActive(true); }
	virtual void	Deactiave_Weapon() { m_pWeapon->IsActive(false); };

	virtual void	Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex = 0, HIT_TYPE eHitType = HIT_END, ATTACK_STRENGTH eAtkStrength = ATK_END) override;
	virtual void	DeActive_CurrentWeaponCollider(_uint iCollIndex = 0) override;

	virtual void    Start_StartDisslove();

	virtual void		Active_Effect(const _uint eType, _bool isLoop = true) override;
	virtual void		DeActive_Effect(const _uint eType) override;
	virtual void		DeActive_AllEffect() override;
	virtual void		Set_Dead_Effect(const _uint eType) override;

	virtual _bool		Get_EffectsLoop(const _uint eType) override;
	virtual void	Reset_WeaponOverlapCheck(_uint iCollIndex = 0) override;

	virtual void	Change_WeaponAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration = 0.2f, _int iStartFrame = 0, _bool bEitherChange = true, _bool bSameChange = false);
	virtual _bool	Get_WeaponAnimEnd(_int iAnimIndex) override;

	virtual const _Matrix* Get_WeaponBoneCombinedMat(_uint iBoneIndex) override;
	virtual const _Matrix* Get_WeaponWorldMat() override;

	virtual void    Start_CutScene(_uint iCutSceneNum) override;
	virtual void    End_CutScene(_uint iCutSceneNum) override;
	virtual void    Change_Model(_uint iModelNum) override;

	void On_Aura(_bool bAura, _bool bCutScene);

	virtual void		Resetting() override;
	virtual void		On_PowerAttack(_bool bOn) override;
private:
	CColliderObject* m_pColliderObject = { nullptr };
	vector<CGameObject*>	CollObjRenderP{};
	vector<class CEffect_Container*> m_Effects;

	class CWeapon*			m_pWeapon = { nullptr };
	class CFsm*				m_pExtraFsmCom = { nullptr };	//2페이즈 fsm
	class CModel*			m_pP1ModelCom = { nullptr };	//1페이즈 model
	class CModel*			m_pExtraModelCom = { nullptr };	//2페이즈 model
	
	class CModel*			m_pCutSceneModelCom[CUTSCENE_END] = { nullptr, };
	class CFsm*				m_pCutSceneFsmCom = { nullptr };

	class CCollider*		m_EXCollider[COLLTYPE_END] = { nullptr, nullptr };

	_bool					m_isChanged = { false };
	_bool					m_isCutScene = { false };
	_bool					m_isDeadFadeOut = { false };
	_bool					m_isDeadFadeIn = { false };
	_bool					m_isDeadFadeOutSet = { false };
	
	const _Matrix*			m_pColliderBindMatrix[CT_END] = { nullptr, nullptr, nullptr};

	CTexture*				m_pDissloveTexture = { nullptr };

	vector<class CDissolve_Effect*> m_DissolveEffects;
	_float					m_fDeadFadeOutTimer = {0.f};
	_float					m_fDeadFadeInTimer = {0.f};

private:
	virtual void		Update_Debuff(_float fTimeDelta) override;

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