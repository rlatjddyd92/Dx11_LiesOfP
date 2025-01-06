#pragma once

#include "Pawn.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CMonster abstract : public CPawn
{
public:
	enum MONSTER_STATE {
		IDLE, GROGY, HITFATAL, PARALYZE, KNOCKBACK, DIE,

		MONSTER_STATE_END
	};

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	void			Reset_Hp() { m_eStat.fHp = m_eStat.fMaxHp; }


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

	_Vec4				Get_TargetPos() { return m_vPosTarget; }
	_Vec4				Get_TargetDir();

public:
	virtual void		Active_Effect(const _uint eType, _bool isLoop = true) {};
	virtual void		DeActive_Effect(const _uint eType) {};
	virtual void		DeActive_AllEffect() {};
	virtual void		Set_Dead_Effect(const _uint eType) {};

	virtual _bool		Get_EffectsLoop(const _uint eType) { return true; }

	void				Look_Player();
	_float				Calc_Distance_XZ();

	virtual void		Active_Weapon() {};
	virtual void		Deactiave_Weapon() {};

	virtual void		Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex = 0, HIT_TYPE eHitType = HIT_END, ATTACK_STRENGTH eAtkStrength = ATK_END) {};
	virtual void		DeActive_CurretnWeaponCollider(_uint iCollIndex = 0) {};

	void				Set_RimLightColor(_Vec4 vColor) { m_vRimLightColor = vColor; }

public:
	virtual void		OnCollisionEnter(CGameObject* pOther) override;
	virtual void		OnCollisionStay(CGameObject* pOther) override;
	virtual void		OnCollisionExit(CGameObject* pOther) override;

	virtual void		Reset_WeaponOverlapCheck(_uint iCollIndex = 0) {};
	virtual void		Change_WeaponAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration = 0.2f, _int iStartFrame = 0, _bool bEitherChange = true, _bool bSameChange = false) {};
	virtual _bool		Get_WeaponAnimEnd(_int iAnimIndex) { return false; }

	virtual const _Matrix* Get_BoneCombinedMat(_uint iBoneIndex);
	virtual const _Matrix* Get_WeaponBoneCombinedMat(_uint iBoneIndex) { return nullptr; }
	virtual const _Matrix* Get_WeaponWorldMat() { return nullptr; }
	
	virtual _bool		Calc_DamageGain(_float fAtkDmg, _Vec3 vHitPos = { 0.f,0.f,0.f }, _uint iHitType = HIT_END, _uint iAttackStrength = ATK_END, CGameObject* pAttacker = nullptr) override;

	void				Increase_GroggyPoint(_float fGroggy) { m_eStat.fGrogyPoint += fGroggy; }
	void				Reset_GroggyPoint() { m_eStat.fGrogyPoint = 0; }
	virtual void		SetUp_Dead() {};
	
	virtual void		Start_Fatal();
	_bool*				Get_bFatalAttacked() { return &m_bFatalAttacked; }

	_bool*				Get_bDicover() { return &m_bDiscover; };
	_bool				Get_TargetDead() { return m_bTargetDead; }
	_bool*				Get_FirstMeetCheck() { return &m_bFirstMeetCheck; };
	_Vec3				Get_Look() { return XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)); }
	_bool				Get_IsWeakness() { return m_eStat.bWeakness; }//해당 위크니스는 각각 몹에서 그로기 양 비교하도록 만들기
	
	virtual void		On_PowerAttack(_bool bOn) {};
	virtual void		Resetting() {};

protected:
	_Vec4				m_vRootMoveStack{};
	_Vec3				m_vCurRootMove{};

	_bool               m_bDiscover = { false };
	_bool               m_bFirstMeetCheck = { false };

	_bool		m_bEndAnim = { false };
	_bool		m_bResetRootMove = { true };
	_bool		m_bRootMoveCtr = { true };

	_bool		m_bFatalAttacked = { false };
	_bool		m_bTargetDead = { false };

	_float		m_fPrevTrackPos{};

	_int		m_iOriginCellNum = {0};

	_Vec4		m_vPosTarget{};

	_bool               m_isBoss = { false };

	_int		m_iErgoPoint = { 0 };

protected:
	HRESULT		Ready_Components();
	HRESULT		Ready_FSM();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END