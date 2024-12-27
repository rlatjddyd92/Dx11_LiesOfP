#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon_SimonManus_Hammer : public CWeapon
{
private:
	CWeapon_SimonManus_Hammer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_SimonManus_Hammer(const CWeapon_SimonManus_Hammer& Prototype);
	virtual ~CWeapon_SimonManus_Hammer() = default;

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

	virtual _bool Active_Collider(_float fDamageRatio = 1.f, _uint iHandIndex = 0, _uint iHitType = 0, _uint iAtkStrength = 0);
	virtual void DeActive_Collider(_uint iHandIndex = 1);


private:
	HRESULT Ready_Components();

private:
	CCollider* m_pExtraColliderCom = { nullptr };

private:
	_bool   m_bStoppedSpin = { true };

public:
	static CWeapon_SimonManus_Hammer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END