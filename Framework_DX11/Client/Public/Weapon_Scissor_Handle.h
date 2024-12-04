#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon_Scissor_Handle : public CWeapon
{
public:
	enum SCISSOR_TYPE { SCISSOR_LEFT, SCISSOR_RIGHT, SCISSOR_END };

	typedef struct : public PLAYER_WAPON_DESC
	{
		SCISSOR_TYPE eScissorType;
	}SCISSOR_DESC;

private:
	CWeapon_Scissor_Handle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Scissor_Handle(const CWeapon_Scissor_Handle& Prototype);
	virtual ~CWeapon_Scissor_Handle() = default;

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

	virtual void		Play_HitSound(HIT_TYPE eType) override;

private:
	class CWeapon_Scissor_Blade*	m_pBlade = { nullptr, };

	SCISSOR_TYPE		m_eType = { SCISSOR_END };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Blade();
	
public:
	static CWeapon_Scissor_Handle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END