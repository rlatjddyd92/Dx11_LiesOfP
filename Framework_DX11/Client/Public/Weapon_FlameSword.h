#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon_FlameSword : public CWeapon
{
private:
	CWeapon_FlameSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_FlameSword(const CWeapon_FlameSword& Prototype);
	virtual ~CWeapon_FlameSword() = default;

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
	HRESULT Ready_Components();

public:
	static CWeapon_FlameSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END