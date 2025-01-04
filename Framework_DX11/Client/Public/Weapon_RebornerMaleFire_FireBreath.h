#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon_RebornerMaleFire_FireBreath : public CWeapon
{
private:
	CWeapon_RebornerMaleFire_FireBreath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_RebornerMaleFire_FireBreath(const CWeapon_RebornerMaleFire_FireBreath& Prototype);
	virtual ~CWeapon_RebornerMaleFire_FireBreath() = default;

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

	virtual void ChangeAnimation(_int iAnimIndex, _bool isLoop, _float fChangeDuration = 0.2f, _int iStartFrame = 0, _bool bEitherChange = true, _bool bSameChange = false) override;
	virtual _bool is_EndAnim(_int iAnimIndex);

public:
	virtual _bool Active_Collider(_float fDamageRatio = 1.f, _uint iHandIndex = 0, HIT_TYPE eHitType = HIT_END, ATTACK_STRENGTH eAtkStrength = ATK_END) override;		//1번 왼손 0번 오른손
	virtual void DeActive_Collider(_uint iHandIndex = 1) override;
private:
	HRESULT Ready_Components();

private:
	_bool   m_bRangeCtl = { true };

	_float	m_fAttackRange = {0.f};

	_float	m_fResetTimer = { 0.f };
	_float	m_fResetTime = { 0.5f };

public:
	static CWeapon_RebornerMaleFire_FireBreath* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END