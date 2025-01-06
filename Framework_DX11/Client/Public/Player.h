#pragma once

#include "Client_Defines.h"
#include "Pawn.h"
#include "PlayerCamera.h"
#include "Item_Enum.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
END

BEGIN(Client)

class CPlayer final : public CPawn
{
public:
	enum WEAPON_TYPE { WEP_RAPIER, WEP_SCISSOR, WEP_FLAME, WEP_END };

	enum DEBUFF_TYPE { DEBUFF_FIRE, DEBUFF_ELEC, DEBUFF_ACID, DEBUFF_END };

	enum EFFECT_TYPE
	{
		EFFECT_GRIND, EFFECT_HEAL,

		EFFECT_ARM_COUNTER_CHARGE, EFFECT_ARM_SHIELDBLOCK,

		EFFECT_CUTSCENE_ARM_OPENDOOR,

		EFFECT_ARM_BUFF,

		EFFECT_ITEM_PURIFICATION, EFFECT_ITEM_RESISTANCE,

		EFFECT_END
	};

	enum PLAYER_STATE
	{
		HIT, PARRY, HEAL, CHANGEWEP, GRINDER, GETUP, THROW_ITEM, DEBUFF_RESISTANCE, DEBUFF_RESET, RETRY_BOSS, DIE,

		OH_IDLE, OH_WALK, OH_RUN, OH_SPRINT, OH_GUARD, OH_GUARDHIT, OH_JUMP, OH_DASH,

		TH_IDLE, TH_WALK, TH_RUN, TH_SPRINT, TH_GUARD, TH_GUARDHIT, TH_JUMP, TH_DASH,

		RAPIER_LATTACK0, RAPIER_LATTACK1, RAPIER_RATTACK0, RAPIER_CHARGE, RAPIER_FABALE, RAPIER_PARRYATTACK, RAPIER_FATAL,

		FLAME_LATTACK0, FLAME_LATTACK1, FLAME_RATTACK0, FLAME_RATTACK1, FLAME_CHARGE0, FLAME_CHARGE1, FLAME_FABLE, FLAME_PARRYATTACK, FLAME_FATAL,

		SCISSOR_LATTACK0, SCISSOR_LATTACK1, SCISSOR_RATTACK0, SCISSOR_RATTACK1, SCISSOR_CHARGE0, SCISSOR_CHARGE1, SCISSOR_FATAL,
		SCISSOR_FABAL0, SCISSOR_FABAL1, SCISSOR_FABAL2, SCISSOR_BUFF,

		ARM_START, ARM_LOOP, ARM_END, ARM_GURAD_WEAK, ARM_GURAD_HARD, ARM_GURAD_HEAVY, ARM_COUNTER, ARM_SWING, ARM_THRUST, ARM_PARRY, ARM_BOMB,

		LADDER = 100, LIFT, CHEST, ITEMGET, STARGAZER, TELEPORT,

		CUTSCENE_IDLE = 200,
		SOPHIA_DOOR_OPEN, SOPHIA_WALK, SOPHIA_HAND, SOPHIA_HANDEND,
		RAXASIA_DOOR_OPEN,

		STATE_END
	};

	enum DISSOLVE_EFFECT
	{
		DISSOLVE_DEAD,
		DISSOLVE_END
	};

	// 24-11-27 김성용
	// 플레이어 스탯 구조체 
#pragma region PLAYER_STAT
	typedef struct PLAYER_STAT_INFO
	{
		void Reset_Zero()
		{
			iPlayer_Level = 0;

			iPoint_HP = 0;
			iPoint_Stamina = 0;
			iPoint_Attack = 0;
			iPoint_Defence = 0;
			iPoint_Heal = 0;

			iStat_Attack = 0;
			iStat_Defence = 0;

			vGauge_Hp = { 0.f,0.f,0.f,0.f };
			vGauge_Stamina = { 0.f,0.f,0.f,0.f };
			vGauge_Region = { 0.f,0.f,0.f,0.f };

			fRegion_Interval = 0.f;

			iErgo = 0;
			iErgo_LevelUp = 0;
			iPoint_Per_Level = 0;

			fDebuff_Fire = { 0.f,0.f };
			fDebuff_Electric = { 0.f,0.f };
			fDebuff_Acid = { 0.f,0.f };
		};

		// 유저 레벨 
		_int iPlayer_Level = 1;

		// 스탯 포인트 (레벨 업 때 찍는 포인트)
		_int iPoint_HP = 1;
		_int iPoint_Stamina = 1;
		_int iPoint_Attack = 1;
		_int iPoint_Defence = 1;
		_int iPoint_Heal = 1;

		// 능력치 
		_float iStat_Attack = 1000.f;
		_float iStat_Defence = 20.f;


		// 게이지 수치 
		// x : 현재 수치
		// y : 보조 수치(HP의 가드리게인 등 표시)
		// z : 현재 최대치 (현재 수치가 늘어날 수 있는 최대치)
		// w : 최대치 한계 (게이지의 최대치를 늘릴 때 늘어날 수 있는 한계치) 
		_Vec4 vGauge_Hp = { 500.f,500.f,500.f,1000.f };
		_Vec4 vGauge_Stamina = { 500.f,500.f,500.f,1000.f };
		_Vec4 vGauge_Region = { 600.f,600.f,600.f,1000.f };

		// 리전 1칸의 수치 
		// UI 리전 1칸에 표시되는 양 
		// 리전 게이지는 _int(현재 최대치) / _int(1칸 수치)의 값 만큼만 나타남 -> 현재 최대치 700, 1칸 수치 200이면 3칸만 뜬다 
		_float fRegion_Interval = 200.f;

		// 에르고 
		_int iErgo = 10000;

		// 레벨 업에 필요한 에르고 
		_int iErgo_LevelUp = 100;

		// 레벨 1 상승 시, 사용 가능한 스탯 포인트 
		_int iPoint_Per_Level = 5;

		// 상태이상 
		// x : 현재 수치 
		// y : 최대치 
		// 현재 수치가 0보다 크면 게이지가 나타난다 
		_Vec2 fDebuff_Fire = { 0.f, 100.f };
		_Vec2 fDebuff_Electric = { 0.f, 100.f };
		_Vec2 fDebuff_Acid = { 0.f, 100.f };

		// 아뮬렛 
		_bool bDebuff_Fire_Ignore = false; // 꿰뚫는 증오의 아뮬렛 : 화염 상태이상 면역 
		_bool bDebuff_Electric_Ignore = false; // 불굴의 아뮬렛 : 전격 상태이상 면역 
		_bool bDebuff_Acid_Ignore = false; // 베테랑의 아뮬렛 : 산성 상태이상 면역 

		_float fHeal = 0.f; // 지속적으로 HP 증가 -> 재충전의 아뮬렛

		_float fResist_Fire = 0.f;
		_float fResist_Electric = 0.f;
		_float fResist_Acid = 0.f;

	}STAT_INFO;
#pragma endregion

public:
	CPlayerCamera*			Get_Camera() { return m_pPlayerCamera; }
	void					Set_Camera(class CPlayerCamera* pCamera) { m_pPlayerCamera = pCamera; }

	_bool					Get_IsJump() { return m_isJump; }
	void					Set_IsJump(_bool isJump) { m_isJump = isJump; }

	_bool					Get_IsGuard() { return m_isGuard; }
	void					Set_IsGuard(_bool isGuard, _bool isReset = true, _float fTimeOffset = 0.f) {
		m_isGuard = isGuard;
		if (m_isGuard && isReset)
		{
			m_isGuardSlow = true;
			m_fGuardTime = 0.f + fTimeOffset;
		}
	}

	class CWeapon_PlayerArm* Get_Arm() { return m_pWeapon_Arm; }

	_bool					Get_IsArm() { return m_isArm; }
	void					Set_IsArm(_bool isArm) {
		m_isArm = isArm;
		if (isArm)
			m_fArmRecoveryTime = 1.f;
	}
	_Vec2*					Get_ArmGuage_Ptr() { return &m_vGuage_Arm; }

	_bool					Get_IsParry() { return m_isParry; }
	void					Set_IsParry(_bool isParry) { m_isParry = isParry; }

	_bool					Get_IsLockOn() { return m_isLockOn; }

	_bool					Get_IsLadderEnd() { return m_isLadderEnd; }

	_bool					Get_IsTeleport() { return m_isTeleport; }
	void					Set_IsTeleport(_bool isTeleport) { m_isTeleport = isTeleport; }

	_bool					Get_IsFatal() { return  m_isFatalAttack; }

	WEAPON_TYPE				Get_WeaponType() { return m_eWeaponType; }
	void					Set_WeaponType(WEAPON_TYPE eType) { m_eWeaponType = eType; }
	class CWeapon*			Get_CurrentWeapon() { return m_pWeapon[m_eWeaponType]; }

	void					Reset_Root() { m_vCurRootMove = m_vRootMoveStack = _vector{ 0,0,0,0 }; }

	class CMonster*			Get_TargetMonster() { return m_pTargetMonster; }

	void					Set_DissloveRatio(_float fRatio) { m_fDissloveRatio = fRatio; }

	void					Set_isPlayingCutscene(_bool isPlaying) { m_isPlayingCutscene = isPlaying; }

	_float					Get_AttackBuffTime() { return m_fAttackBuffTime; }
	void					Set_AttackBuffTime(_float fTime) { m_fAttackBuffTime = fTime; }

	_bool					Get_IsRespawnMonster() { return m_isRespawnMonster; }
	void					Set_IsRespawnMonster(_bool isRespawn) { m_isRespawnMonster = isRespawn; }

#pragma region PLAYER_STAT
	STAT_INFO& Get_Player_Stat() { return *m_tPlayer_Stat; }
	// 24-12-05 김성용
	// UI 매니저 수정용 함수
	STAT_INFO* Get_Player_Stat_Adjust() { return m_tPlayer_Stat_Adjust; }

#pragma endregion


private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

public:
	virtual void                SetUp_Die() override;
	virtual void			    Reset_Die() override;

public:
	_bool			Key_Tab(KEY eKey);
	_bool			Key_Hold(KEY eKey);
	_bool			Key_Away(KEY eKey);
	_bool			Key_None(KEY eKey);

	virtual	void	Change_State(_uint iState, void* pArg = nullptr);

	void			Move_Dir(_Vec4 vDir, _float fTimeDelta, _bool isTurn = true);
	_bool			Turn_Lerp(_Vec4 vDir, _float fTimeDelta);

	_Vec4			Calculate_Direction_Straight();
	_Vec4			Calculate_Direction_Right();

	void			Change_Weapon();
	_uint			Change_WeaponType();
	void			Appear_Weapon();
	void			Disappear_Weapon();
	void			Set_WeaponStrength(ATTACK_STRENGTH eStrength);

	_bool			Active_CurrentWeaponCollider(_float fDamageRatio = 1.f, _uint iHandIndex = 1);
	void			DeActive_CurretnWeaponCollider(_uint iHandIndex = 1);

	_bool			Active_Arm();
	void			DeActive_Arm();

	void			Seperate_Scissor();
	void			Combine_Scissor(_bool isForce = false);

	void			Change_CameraMode(CPlayerCamera::CAMERA_MODE eMode);

	void			Toggle_LockOn();
	void			Off_LockOn();
	CMonster*		Find_TargetMonster();

	void			Play_CurrentWeaponSound(const _uint iType, const TCHAR* pSoundKey, _uint iHandIndex = 1);

	void			Active_Effect(const EFFECT_TYPE& eType, _bool isLoop = true);
	void			DeActive_Effect(const EFFECT_TYPE& eType);

	void			Active_WeaponEffect(_uint iEffectType, _bool isLoop = true, _uint iHandIndex = 0);
	void			DeActive_WeaponEffect(_uint iEffectType, _uint iHandIndex = 0);

	virtual _bool	Calc_DamageGain(_float fAtkDmg, _Vec3 vHitPos = { 0.f,0.f,0.f }, _uint iHitType = HIT_END, _uint iAttackStrength = ATK_END, CGameObject* pAttacker = nullptr) override;
	void			Calc_DebuffGain(_uint iDebuffType, _float fAmount);
	void			Damaged(_float fAtkDmg);
	void			Damaged_Guard(_float fAtkDmg, const _Matrix* pSocketBoneMatrix = nullptr);
	void			DotDamaged(_float fDamageAmount);
	void			Change_HitState(_float fAtkDmg, _Vec3 vHitPos, _uint iAttackStrength);
	_bool			Decrease_Stamina(_float fAmount);
	_bool			Check_Region_Fable01();
	_bool			Check_Region_Fable02();
	void			Increase_Region(_float fAmount);
	void			Decrease_Region(_uint iRegionCount = 1);
	void			Recovery_Region(_float fAmount = 10.f);
	void			Recovery_HP(_float fAmount);
	void			Decrease_Arm(_float fAmount = 0.1f);

	class CStargazer* Find_Stargazer(_int iCellNumber = -1);

	void			Create_ThrowItem(SPECIAL_ITEM eItemType);
	void			Throw_ITem();

	void			Use_DebuffReduceItem();
	void			Use_DebuffResetItem();

	void			Init_PlayerCamera();
	void			Input_Level_UP_Stat(_int iPoint[5], _float pLevelUpStat[9]);

	void			On_DissolveEffect(_uint iIndex, _bool bOn);

	void			SetUp_Monster_Fatal();

private:
	_bool					m_bActivated_ManusCutScene = { false };
	_bool					m_isPlayingCutscene = { false };

private:
	vector<class CEffect_Container*> m_Effects;
	class CEffect_Manager* m_pEffect_Manager = { nullptr };

	CTexture* m_pDissloveTexture = { nullptr };

private:
	CPlayerCamera* m_pPlayerCamera = { nullptr };

	_bool				m_isJump = { false };
	_bool				m_isGuard = { false };
	_bool				m_isGuardSlow = { false };
	_bool				m_isArm = { false };
	_bool				m_isParry = { false };
	_bool				m_isLockOn = { false };
	_bool				m_isRecoveryStamina = { false };
	_bool				m_isLadderEnd = { false };
	_bool				m_isTeleport = { false };
	_bool				m_isCollisionMonster = { false };
	_bool				m_isRespawnMonster = { false };

	_float				m_fGuardTime = {};
	_float				m_fDebuffSpeedRatio = { 1.f };
	_float				m_fDebuffAcidDamageTime = {};
	_float				m_fRecoveryStaminaTime = { 0.f };
	_float				m_fRecoveryArmTime = { 0.f };

	CMonster* m_pTargetMonster = { nullptr };
	CMonster* m_pIntersectMonster = { nullptr };

	class CWeapon* m_pWeapon[WEP_END] = { nullptr, };
	WEAPON_TYPE			m_eWeaponType = { WEP_RAPIER };

	class CWeapon_PlayerArm* m_pWeapon_Arm = { nullptr };

	class CItem_Throw* m_pThrowItem = { nullptr };
	vector<class CDissolve_Effect*> m_DissolveEffects;

	class CPlayerCollider_Fatal* m_pFatalColliderObj = { nullptr };
	class CMonster* m_pContactMonster = { nullptr };
	_bool							m_isFatalAttack = { false };

private:
	_vector		m_vRootMoveStack = {};
	_Vec3		m_vCurRootMove = {};
	_double		m_PrevTrackPos = {};
	_bool		m_bEndAnim = { false };
	_bool		m_bResetRootMove = { true };

	// 24-11-27 김성용
	// 플레이어 스탯 구조체 변수 
#pragma region PLAYER_STAT
	STAT_INFO* m_tPlayer_Stat = { nullptr };
	STAT_INFO* m_tPlayer_Stat_Adjust = { nullptr };	// 
#pragma endregion

private:
	_float				m_fStaminaRecoveryTime = {};

	_float				m_fDebuffReduceTime = {};
	_float				m_fDebuffRecoveryTime[DEBUFF_END] = {};

	_float				m_fArmRecoveryTime = {};
	_Vec2				m_vGuage_Arm = { 200.f, 200.f };

	_float				m_fAttackBuffTime = {};

	_bool				m_isDotDamage = {};
	_float				m_fDotDamgeAmount = {};

private:

	void			Update_Stat(_float fTimeDelta);
	void			CollisionStay_IntercObj(CGameObject* pGameObject);
	void			Choice_GuardSound(_uint iAttackStrength = ATK_WEAK, _uint iHitType = HIT_END, _bool isPerfect = false);
	void			Update_PrevItemInfo();
	void			Check_FatalAttack();

private:
	HRESULT Ready_Components();
	HRESULT Ready_Weapon();
	HRESULT Ready_FSM();
	HRESULT Ready_Effect();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};


END