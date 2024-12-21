#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon_PlayerArm : public CWeapon
{
private:
	CWeapon_PlayerArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_PlayerArm(const CWeapon_PlayerArm& Prototype);
	virtual ~CWeapon_PlayerArm() = default;

public:
	_Vec3 Get_Position() { return m_WorldMatrix.Translation(); }

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

public:
	static CWeapon_PlayerArm* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END