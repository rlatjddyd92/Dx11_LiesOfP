#pragma once

#include "Client_Defines.h"
#include "Pawn.h"
#include "PlayerCamera.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
END

BEGIN(Client)

class CPlayer final : public CPawn
{
public:
	enum WEAPON_TYPE { WEP_RAPIER, WEP_SCISSOR, WEP_FLAME, WEP_END };

	enum EFFECT_TYPE 
	{
		RAPIER_TRAIL_FIRST, RAPIER_TRAIL_SECOND,

		EFFECT_END
	};

	enum PLAYER_STATE
	{
		HIT, PARRY, HEAL, CHANGEWEP, LADDER, LIFT,

		OH_IDLE, OH_WALK, OH_RUN, OH_SPRINT, OH_GUARD, OH_JUMP, OH_DASH,

		TH_IDLE, TH_WALK, TH_RUN, TH_SPRINT, TH_GUARD, TH_JUMP, TH_DASH,

		RAPIER_LATTACK0, RAPIER_LATTACK1, RAPIER_RATTACK0, RAPIER_CHARGE, RAPIER_FATAL, RAPIER_PARRYATTACK,

		FLAME_LATTACK0, FLAME_LATTACK1, FLAME_RATTACK0, FLAME_RATTACK1, FLAME_CHARGE0, FLAME_CHARGE1, FLAME_FATAL, FLAME_PARRYATTACK,

		SCISSOR_LATTACK0, SCISSOR_LATTACK1, SCISSOR_RATTACK0, SCISSOR_RATTACK1, SCISSOR_CHARGE0, SCISSOR_CHARGE1, 
		SCISSOR_FATAL0, SCISSOR_FATAL1, SCISSOR_FATAL2, SCISSOR_BUFF,

		STATE_END
	};

	// 24-11-27 김성용
	// 플레이어 스탯 구조체 
#pragma region PLAYER_STAT
	typedef struct PLAYER_STAT_INFO
	{
		// 스탯 
		_int iStat_HP = 1;
		_int iStat_Stamina = 1;
		_int iStat_Attack = 1;
		_int iStat_Defence = 1;
		_int iStat_Heal = 1;

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
		_int iErgo = 1000;

		// 상태이상 
		// x : 현재 수치 
		// y : 최대치 
		// 현재 수치가 0보다 크면 게이지가 나타난다 
		_Vec2 fDebuff_Fire = { 0.f, 100.f };
		_Vec2 fDebuff_Electric = { 0.f, 100.f };
		_Vec2 fDebuff_Acid = { 0.f, 100.f };
	}STAT_INFO;
#pragma endregion

public:
	CPlayerCamera*			Get_Camera() { return m_pPlayerCamera; }
	void					Set_Camera(class CPlayerCamera* pCamera) { m_pPlayerCamera = pCamera; }

	_bool					Get_IsJump() { return m_isJump; }
	void					Set_IsJump(_bool isJump) { m_isJump = isJump; }

	_bool					Get_IsGuard() { return m_isGuard; }
	void					Set_IsGuard(_bool isGuard) {
		m_isGuard = isGuard;
		if (m_isGuard)
			m_fGuardTime = 0.f;
	}

	_bool					Get_IsParry() { return m_isParry; }
	void					Set_IsParry(_bool isParry) { m_isParry = isParry; }

	_bool					Get_IsLockOn() { return m_isLockOn; }
	void					Set_IsLockOn(_bool isLockOn) { m_isLockOn = isLockOn; }

	_bool					Get_IsInvicible() { return m_isInvicible; }
	void					Set_IsInvicible(_bool isInvicible) { m_isInvicible = isInvicible; }

	_bool					Get_IsLadderEnd() { return m_isLadderEnd; }

	WEAPON_TYPE				Get_WeaponType() { return m_eWeaponType; }
	void					Set_WeaponType(WEAPON_TYPE eType) { m_eWeaponType = eType; }
	class CWeapon*			Get_CurrentWeapon() { return m_pWeapon[m_eWeaponType]; }

	void					Reset_Root() { m_vCurRootMove = m_vRootMoveStack = _vector{0,0,0,0}; }

	CPawn*					Get_TargetMonster() { return m_pTargetMonster; }

	// 24-11-27 김성용
	// 플레이어 스탯 구조체 접근 함수 
	// 외부에서는 레퍼런스로 접근 
	// 수치 조정은 반드시 플레이어에서만 진행 
#pragma region PLAYER_STAT
	const STAT_INFO&		Get_Player_Stat() const { return *m_tPlayer_Stat; }

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
	void			Move_Dir(_Vec4 vDir, _float fTimeDelta, _bool isTurn = true);
	_Vec4			Calculate_Direction_Straight();
	_Vec4			Calculate_Direction_Right();

	void			Change_Weapon();
	_uint			Change_WeaponType();
	
	void			Active_CurrentWeaponCollider(_float fDamageRatio = 1.f, _uint iHandIndex = 1);
	void			DeActive_CurretnWeaponCollider(_uint iHandIndex = 1);

	void			Seperate_Scissor();
	void			Combine_Scissor();

	void			Chnage_CameraMode(CPlayerCamera::CAMERA_MODE eMode);

	void			LockOnOff();
	CPawn*			Find_TargetMonster();

	void			Play_CurrentWeaponSound(const _uint iType, const TCHAR* pSoundKey, _uint iHandIndex = 1);

	void			Active_Effect(const EFFECT_TYPE& eType);
	void			DeActive_Effect(const EFFECT_TYPE& eType);

	virtual _bool	Calc_DamageGain(_float fAtkDmg, _Vec3 vHitPos = { 0.f,0.f,0.f }) override;

private:
	vector<class CEffect_Container*> m_Effects;

private:
	CPlayerCamera*		m_pPlayerCamera = { nullptr };

	_bool				m_isJump = { false };
	_bool				m_isGuard = { false };
	_bool				m_isParry = { false };
	_bool				m_isLockOn = { false };
	_bool				m_isInvicible = { false };
	_bool				m_isRecoveryStamina = { false };
	_bool				m_isLadderEnd = { false };

	_float				m_fGuardTime = {};
	_float				m_fRecoveryStaminaTime = { 0.f };

	CPawn*				m_pTargetMonster = { nullptr };

	class CWeapon*		m_pWeapon[WEP_END] = { nullptr, };
	WEAPON_TYPE			m_eWeaponType = { WEP_RAPIER };

private:
	_vector		m_vRootMoveStack = {};
	_Vec3		m_vCurRootMove = {};
	_double		m_PrevTrackPos = {};
	_bool		m_bEndAnim = { false };
	_bool		m_bResetRootMove = { true };

	// 24-11-27 김성용
	// 플레이어 스탯 구조체 변수 
#pragma region PLAYER_STAT
	STAT_INFO*	m_tPlayer_Stat = { nullptr };
#pragma endregion

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