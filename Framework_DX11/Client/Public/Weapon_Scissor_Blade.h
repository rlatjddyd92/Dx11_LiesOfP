#pragma once

#include "Client_Defines.h"
#include "Weapon.h"
#include "Weapon_Scissor_Handle.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon_Scissor_Blade : public CWeapon
{
private:
	CWeapon_Scissor_Blade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Scissor_Blade(const CWeapon_Scissor_Blade& Prototype);
	virtual ~CWeapon_Scissor_Blade() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth() override;

private:
	CWeapon_Scissor_Handle::SCISSOR_TYPE		m_eType = { CWeapon_Scissor_Handle::SCISSOR_END };

private:
	HRESULT Ready_Components();

public:
	static CWeapon_Scissor_Blade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END