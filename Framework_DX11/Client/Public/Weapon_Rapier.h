#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon_Rapier : public CWeapon
{
public:
	enum EFFECT_TYPE
	{
		EFFECT_STORMSTAB1, EFFECT_STORMSTAB2, EFFECT_END
	};

private:
	CWeapon_Rapier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Rapier(const CWeapon_Rapier& Prototype);
	virtual ~CWeapon_Rapier() = default;

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
	virtual void		Play_HitSound(HIT_TYPE eType) override;

private:
	HRESULT Ready_Components();
	HRESULT Ready_Effect();

public:
	static CWeapon_Rapier* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END