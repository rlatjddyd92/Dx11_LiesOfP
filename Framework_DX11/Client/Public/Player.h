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

	enum PLAYER_STATE
	{
		HIT, PARRY, HEAL, CHANGEWEP,

		OH_IDLE, OH_WALK, OH_RUN, OH_SPRINT, OH_GUARD, OH_JUMP, OH_DASH,

		TH_IDLE, TH_WALK, TH_RUN, TH_SPRINT, TH_GUARD, TH_JUMP, TH_DASH,

		RAPIER_LATTACK0, RAPIER_LATTACK1, RAPIER_RATTACK0, RAPIER_CHARGE, RAPIER_FATAL, RAPIER_PARRYATTACK,

		FLAME_LATTACK0, FLAME_LATTACK1, FLAME_RATTACK0, FLAME_RATTACK1, FLAME_CHARGE0, FLAME_CHARGE1, FLAME_FATAL, FLAME_PARRYATTACK,

		SCISSOR_LATTACK0, SCISSOR_LATTACK1, SCISSOR_RATTACK0, SCISSOR_RATTACK1, SCISSOR_CHARGE0, SCISSOR_CHARGE1, 
		SCISSOR_FATAL0, SCISSOR_FATAL1, SCISSOR_FATAL2, SCISSOR_BUFF,

		STATE_END
	};

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

	WEAPON_TYPE				Get_WeaponType() { return m_eWeaponType; }
	void					Set_WeaponType(WEAPON_TYPE eType) { m_eWeaponType = eType; }
	class CWeapon*			Get_CurrentWeapon() { return m_pWeapon[m_eWeaponType]; }

	void					Reset_Root() { m_vCurRootMove = m_vRootMoveStack = _vector{0,0,0,0}; }

	CPawn*					Get_TargetMonster() { return m_pTargetMonster; }

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

private:
	list<class CEffect_Container*>	m_EffectList;
	map<_uint, class CEffect_Container*>	m_Effects;

private:
	CPlayerCamera*		m_pPlayerCamera = { nullptr };

	_bool				m_isJump = { false };
	_bool				m_isGuard = { false };
	_bool				m_isParry = { false };
	_bool				m_isLockOn = { false };
	_bool				m_isInvicible = { false };

	_float				m_fGuardTime = {};

	CPawn*				m_pTargetMonster = { nullptr };

	class CWeapon*		m_pWeapon[WEP_END] = { nullptr, };
	WEAPON_TYPE			m_eWeaponType = { WEP_RAPIER };

private:
	_vector		m_vRootMoveStack = {};
	_Vec3		m_vCurRootMove = {};
	_double		m_PrevTrackPos = {};
	_bool		m_bEndAnim = { false };
	_bool		m_bResetRootMove = { true };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Weapon();
	HRESULT Ready_FSM();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};


END