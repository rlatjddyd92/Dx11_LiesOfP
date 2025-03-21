#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon abstract : public CGameObject
{
public:
	enum WEP_SOUND_TYPE { WEP_SOUND_WEAPON, WEP_SOUND_EFFECT1, WEP_SOUND_EFFECT2, WEP_SOUND_END };

	typedef struct
	{
		const _Matrix*	pParentWorldMatrix = { nullptr };
		const _Matrix*	pSocketBoneMatrix = { nullptr };
		const _Matrix*	pSocketBoneMatrix2 = { nullptr };

		_float*		pParentAtk = { nullptr };
	}WEAPON_DESC;

	typedef struct : public WEAPON_DESC
	{
		class CPlayer* pPlayer = { nullptr };
	}PLAYER_WAPON_DESC;

	typedef struct : public WEAPON_DESC
	{
		class CMonster* pMonster = { nullptr };

		// 속성 이넘, 기본 속성 스탯 받아오고,
		_uint	iDebuffType = { 3 };
		_float	fDebuffAmount = { 0.f };

	}MONSTER_WAPON_DESC;

protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

public:
	class CModel*		Get_Model() { return m_pModelCom; }
	class CCollider*	Get_Collider() { return m_pColliderCom; }

	class CSound*		Get_SoundCom(WEP_SOUND_TYPE eType) { return m_pSoundCom[eType]; }

	class CPlayer*		Get_Player() { return m_pPlayer; }
	class CMonster*		Get_Monster() { return m_pMonster; }

	void				Set_IsUpdatePos(_bool isUpdatePos) { m_isUpdatePos = isUpdatePos; }

	ATTACK_STRENGTH		Get_AttackStrength() { return m_eAttackStrength; }
	virtual void		Set_AttackStrength(ATTACK_STRENGTH eStrength) { m_eAttackStrength = eStrength; }

	_float				Get_DamageAmount() { return m_fDamageAmount; }
	virtual void		Set_DamageAmount(_float fDamageAmount) { m_fDamageAmount = fDamageAmount; }

	_float Get_AtkDmg() { return m_fDamageAmount * m_fDamageRatio; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;


public:
	virtual void OnCollisionEnter(CGameObject* pOther) {};
	virtual void OnCollisionStay(CGameObject* pOther) {};
	virtual void OnCollisionExit(CGameObject* pOther) {};

	virtual void ChangeAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration = 0.2f, _int iStartFrame = 0, _bool bEitherChange = true, _bool bSameChange = false) {};
	virtual _bool is_EndAnim(_int iAnimIndex) { return false; }

public:
	virtual _bool Active_Collider(_float fDamageRatio = 1.f, _uint iHandIndex = 0, HIT_TYPE eHitType = HIT_END, ATTACK_STRENGTH eAtkStrength = ATK_END);		//1번 왼손 0번 오른손
	virtual void DeActive_Collider(_uint iHandIndex = 1);

	virtual void		Play_Sound(WEP_SOUND_TYPE eType, const TCHAR* pSoundKey, _uint iHandIndex = 1);

	virtual void		Play_HitSound(HIT_TYPE eType) {};

	virtual void		Active_Effect(const _uint& iType, _bool isLoop = true, _uint iHandIndex = 0);  //1번 왼손 0번 오른손
	virtual void		DeActive_Effect(_uint iType, _uint iHandIndex = 0);
	virtual void		DeActive_AllEffect(_uint iHandIndex = 0);

	virtual void		Active_Debuff(_float fDebuffRatio = 1.f);
	virtual void		DeActive_Debuff();

public:
	void Appear();
	void Disappear();


	void ChangeSocketMatrix(const _Matrix* pMat) { m_pSocketMatrix = pMat; }
	void Reset_OverLapCheck() { m_DamagedObjects.clear(); }

	const _Matrix* Get_BoneCombinedMatrix(_uint iBoneIndex);
	const _Matrix* Get_WorldMatrix_Ptr() { return &m_WorldMatrix; }

protected:
	CShader*			m_pShaderCom = { nullptr };	
	CModel*				m_pModelCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };
	class CSound*		m_pSoundCom[WEP_SOUND_END] = {nullptr,};
	_float				m_fEmissiveMask = {};

protected:
	vector<class CEffect_Container*> m_Effects;
	class CEffect_Manager* m_pEffect_Manager = { nullptr };

protected:
	class CPlayer*		m_pPlayer = { nullptr };
	class CMonster*		m_pMonster = { nullptr };

protected:
	const _Matrix*			m_pParentMatrix = { nullptr };
	const _Matrix*			m_pSocketMatrix = { nullptr };
	const _Matrix*			m_pSocketMatrix2 = { nullptr };
	_Matrix					m_WorldMatrix = {};
	_Matrix					m_OldWroldMatrix = {};

	const _Matrix*			m_pBladeMatrix = { nullptr };

	_Matrix					m_BladeWorldMatrix = {};
	_Matrix					m_BladeOldWroldMatrix = {};

protected:
	_float					m_fDamageAmount = {};
	_float					m_fDamageRatio = { 1.f };
	_float					m_fFinalDamageAmount = {};


	_float					m_fDebuffAmount = {0.f};
	_float					m_fDebuffRatio = {1.f};
	_uint					m_iDebuffType = { 3 };

	_bool					m_bDebuffAttack = { false };

	_bool					m_isUpdatePos = { true };

	list<CGameObject*>			m_DamagedObjects;

	_Vec3					m_vVelocity = {};
	_Vec3					m_vAttackDir = {};

	_float*					m_pParentAtk = { nullptr };


	HIT_TYPE				m_eHitType{};
	ATTACK_STRENGTH			m_eAttackStrength = { ATK_WEAK };

	_float					m_fGrogyAmount = {};

protected:
	HRESULT Ready_Components();
	HRESULT Ready_Effect();

	HRESULT Bind_WorldMatrix(class CShader* pShader, const _char* pContantName);
	HRESULT Bind_OldWorldMatrix(class CShader* pShader, const _char* pContantName);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END