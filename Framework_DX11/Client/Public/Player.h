#pragma once

#include "Client_Defines.h"
#include "Pawn.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
END

BEGIN(Client)

class CPlayer final : public CPawn
{
public:
	enum PARTID { PART_BODY, PART_WEAPON, PART_END };

	enum WEAPON_TYPE { WEP_RAPIER, WEP_SCISSOR, WEP_FLAME, WEP_END };

	enum PLAYER_STATE
	{
		HIT,
		OH_IDLE, OH_WALK, OH_RUN, OH_GUARD, OH_JUMP, OH_DASH, 
		RAPIER_NA1, RAPIER_NA2, RAPIER_SA1,
		STATE_END
	};

public:
	class CCamera*			Get_Camera() { return m_pPlayerCamera; }
	void					Set_Camera(class CCamera* pCamera) { m_pPlayerCamera = pCamera; }

	_bool					Get_IsJump() { return m_isJump; }
	void					Set_IsJump(_bool isJump) { m_isJump = isJump; }

	_bool					Get_IsGuard() { return m_isGuard; }
	void					Set_IsGuard(_bool isGuard) {
		m_isGuard = isGuard;
		if (m_isGuard)
			m_fGuardTime = 0.f;
	}

	_bool					Get_IsLockOn() { return m_isLockOn; }
	void					Set_IsLockOn(_bool isLockOn) { m_isLockOn = isLockOn; }

	_bool					Get_IsInvicible() { return m_isInvicible; }
	void					Set_IsInvicible(_bool isInvicible) { m_isInvicible = isInvicible; }

	WEAPON_TYPE				Get_WeaponType() { return m_eWeaponType; }
	void					Set_WeaponType(WEAPON_TYPE eType) { m_eWeaponType = eType; }

	void					Reset_Root() { m_vCurRootMove = m_vRootMoveStack = _vector{0,0,0,0}; }



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
	void		Move_Dir(_Vec4 vDir, _float fSpeed, _float fTimeDelta, _bool isTurn = true);
	_Vec4		Calculate_Direction_Straight();
	_Vec4		Calculate_Direction_Right();

private:
	list<OUTPUT_EVKEY>	m_EvKeyList;
	list<class CEffect_Container*>	m_EffectList;
	map<_uint, class CEffect_Container*>	m_Effects;

private:
	class CCamera*		m_pPlayerCamera = { nullptr };

	_bool				m_isJump = { false };
	_bool				m_isGuard = { false };
	_bool				m_isLockOn = { false };
	_bool				m_isInvicible = { false };

	_float				m_fGuardTime = {};

	WEAPON_TYPE			m_eWeaponType = { WEP_RAPIER };

private:
	_vector		m_vRootMoveStack = {};
	_vector		m_vCurRootMove = {};
	_float		m_fPrevTrackPos = {};
	_bool		m_bEndAnim = { false };
	_bool		m_bResetRootMove = { true };


private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Ready_FSM();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CPawn* Clone(void* pArg);
	virtual void Free() override;

};


END