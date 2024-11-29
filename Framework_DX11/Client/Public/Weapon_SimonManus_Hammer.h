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

private:

private:
	HRESULT Ready_Components();

public:
	static CWeapon_SimonManus_Hammer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END