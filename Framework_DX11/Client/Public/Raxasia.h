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

class CRaxasia final : public CMonster
{
public:
	typedef struct
	{
		_bool* pIsEndAnim{ nullptr };
		_bool* pIsResetRootMove{ nullptr };
		_bool* pColliderCtrs{ nullptr };
		_bool* pRootMoveCtrs{ nullptr };

		_double* pGrogyTrackPos{ nullptr };
	}FSMSTATE_DESC;

public:
	enum COLLIDERTYPE { CT_UPPERBODY, CT_LOWERBODY
		, CT_UPPERLEG_LEFT, CT_UPPERLEG_RIGHT
		, CT_LOWERLEG_LEFT, CT_LOWERLEG_RIGHT
		, CT_UPPERARM_LEFT, CT_UPPERARM_RIGHT
		, CT_LOWERARM_LEFT, CT_LOWERARM_RIGHT, CT_END };
	enum EXCOLLIDER { LOWERBODY, U_LEG_LEFT, U_LEG_RIGHT, L_LEG_LEFT, L_LEG_RIGHT
		, U_ARM_LEFT, U_ARM_RIGHT, L_ARM_LEFT, L_ARM_RIGHT, COLLTYPE_END };
	enum CUTSCENE_MODEL { MODEL_PHASE1, MODEL_PHASE2, MODEL_END };
	enum CUTSCENE_TYPE { CUTSCENE_MEET, CUTSCENE_P2, CUTSCENE_DIE, CUTSCENE_END };

public:
	enum RAXASIA_P1_STATE {
		ATK_DASHUPPER = MONSTER_STATE_END, ATK_GROUNDSLASH, ATK_LINKEDATTACK, ATK_REPETUPPERSLASH
		,ATK_KICKSTING, ATK_TRIPLESTING
		,ATK_DISCHARGE, ATK_JUMPATTACK, ATK_SWINGDOWN_MULTIPLE
	};

	enum RAXASIA_P2_STATE {
		ATKP2_DECLAREWAR = MONSTER_STATE_END, ATKP2_JUMPSTAMP, ATKP2_STEPJUMP
		, ATKP2_RUNNING, ATKP2_RUNNING_LINKED, ATKP2_RUNNING_FURY, ATKP2_WAVESTING
		, ATKP2_TELE, ATKP2_TELE_JUMPLIGHTNING, ATKP2_TELE_LINKEDTEL, ATKP2_TELEPORTATTACK
		, ATKP2_GUARD, ATKP2_SHIELDLINKED, ATKP2_GUARD_PUSH, ATKP2_SHIELDRUSH, ATKP2_CHARGERUSH
	};

	enum RAXASIA_CUTSCENE_MEET_STATE {
		STATE_MEET, STATE_P2, STATE_DIE, STATE_END
	};

	enum EFFECT_TYPE
	{
		
		EFFECT_END
	};

private:
	CRaxasia(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRaxasia(const CRaxasia& Prototype);
	virtual ~CRaxasia() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void	ChangePhase();

	virtual void	Active_CurrentWeaponCollider(_float fDamageRatio, _uint iCollIndex = 0, _uint iHitType = 0, _uint iAtkStrength = 0) override;
	virtual void	DeActive_CurretnWeaponCollider(_uint iCollIndex = 0) override;

	virtual void		Active_Effect(const _uint eType, _bool isLoop = true) override;
	virtual void		DeActive_Effect(const _uint eType) override;

	virtual _bool		Get_EffectsLoop(const _uint eType) override;
	virtual void	Reset_WeaponOverlapCheck(_uint iCollIndex = 0) override;

	virtual void	Change_WeaponAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration = 0.2f, _int iStartFrame = 0, _bool bEitherChange = true, _bool bSameChange = false);
	virtual _bool	Get_WeaponAnimEnd(_int iAnimIndex) override;

	virtual const _Matrix* Get_WeaponBoneCombinedMat(_uint iBoneIndex) override;
	virtual const _Matrix* Get_WeaponWorldMat() override;

	virtual void    Start_CutScene(_uint iCutSceneNum) override;
	virtual void    End_CutScene(_uint iCutSceneNum) override;
	virtual void    Change_Model(_uint iModelNum) override;

private:
	vector<CGameObject*>	CollObjRenderP{};
	vector<class CEffect_Container*> m_Effects;

	CColliderObject* m_pKickCollObj = { nullptr };

	class CWeapon* m_pWeapon = { nullptr };
	class CWeapon* m_pWeaponShield = { nullptr };
	class CFsm* m_pExtraFsmCom = { nullptr };	//2페이즈 fsm
	class CModel* m_pP1ModelCom = { nullptr };	//1페이즈 model
	class CModel* m_pExtraModelCom = { nullptr };	//2페이즈 model

	class CModel* m_pCutSceneModelCom[CUTSCENE_END] = { nullptr, };
	class CFsm* m_pCutSceneFsmCom = { nullptr };

	class CCollider* m_EXCollider[COLLTYPE_END] = { nullptr, nullptr };

	_bool					m_isChanged = { false };
	_bool					m_isCutScene = { false };

	const _Matrix* m_pColliderBindMatrix[CT_END] = { nullptr, nullptr, nullptr };


private:

	HRESULT Ready_Components();
	HRESULT Ready_FSM();
	HRESULT Ready_Weapon();
	HRESULT Ready_Effects();

	void	Update_Collider();


public:
	static CRaxasia* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END