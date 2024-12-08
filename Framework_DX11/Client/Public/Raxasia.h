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
	enum COLLIDERTYPE { CT_UPPERBODY, CT_LOWERBODY, CT_UPPERLEG_LEFT, CT_UPPERLEG_RIGHT
		, CT_LOWERLEG_LEFT, CT_LOWERLEG_RIGHT, CT_UPPERARM_LEFT, CT_UPPERARM_RIGHT
		, CT_LOWERARM_LEFT, CT_LOWERARM_RIGHT, CT_END };
	enum EXCOLLIDER { LEG_LEFT, LEG_RIGHT, LOWERBODY, COLLTYPE_END };

public:
	enum SIMONMANUS_P1_STATE {
		ATK_AVOIDSWING = MONSTER_STATE_END,
	};

	enum SIMONMANUS_P2_STATE {
		ATKP2_AVOIDSWING = MONSTER_STATE_END,
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

private:
	vector<CGameObject*>	CollObjRenderP{};
	vector<class CEffect_Container*> m_Effects;

	class CWeapon* m_pWeapon = { nullptr };
	class CFsm* m_pExtraFsmCom = { nullptr };	//2페이즈 fsm
	class CModel* m_pExtraModelCom = { nullptr };	//2페이즈 model

	class CCollider* m_EXCollider[COLLTYPE_END] = { nullptr, nullptr };

	_bool					m_isChanged = { false };

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