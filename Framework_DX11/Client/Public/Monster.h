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
		IDLE, WALK, RUN, ATTACK, GROGY, HITFATAL, PARALYZE, DIE,

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

	_Vec4		Get_TargetPos() { return m_vPosTarget; }
	_Vec4		Get_TargetDir();

public:
	virtual void		Active_Effect(const _uint eType, _bool isLoop = true) {};
	virtual void		DeActive_Effect(const _uint eType) {};
	virtual void		DeActive_AllEffect() {};

	virtual _bool		Get_EffectsLoop(const _uint eType) = 0;

	void		Set_UpTargetPos();
	void		Look_Player();
	_float		Calc_Distance_XZ();

	virtual void	Active_Weapon() {};
	virtual void	Deactiave_Weapon() {};

	virtual void	Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex = 0, _uint iHitType = 0, _uint iAtkStrength = 0);
	virtual void	DeActive_CurretnWeaponCollider(_uint iCollIndex = 0);

	void		Set_RimLightColor(_Vec4 vColor) { m_vRimLightColor = vColor; }

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

	virtual void	Reset_WeaponOverlapCheck(_uint iCollIndex = 0) {};
	virtual void	Change_WeaponAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration = 0.2f, _int iStartFrame = 0, _bool bEitherChange = true, _bool bSameChange = false) {};
	virtual _bool	Get_WeaponAnimEnd(_int iAnimIndex) { return false; }

	virtual const _Matrix* Get_BoneCombinedMat(_uint iBoneIndex);
	virtual const _Matrix* Get_WeaponBoneCombinedMat(_uint iBoneIndex) { return &_Matrix{}; }
	virtual const _Matrix* Get_WeaponWorldMat() { return &_Matrix{}; }

	void	Increase_GroggyPoint(_float fGroggy) { m_eStat.fGrogyPoint += fGroggy; }
	virtual void	SetUp_Dead() {};

protected:
	_Vec4		m_vRootMoveStack{};
	_Vec3		m_vCurRootMove{};
	_bool		m_bEndAnim{ false };
	_bool		m_bResetRootMove{ true };
	_bool		m_bRootMoveCtr{ true };

	_float		m_fPrevTrackPos{};

	_Vec4		m_vPosTarget{};

protected:
	HRESULT Ready_Components();
	HRESULT Ready_FSM();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END