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

class CWeapon_Scissor : public CWeapon
{
private:
	CWeapon_Scissor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Scissor(const CWeapon_Scissor& Prototype);
	virtual ~CWeapon_Scissor() = default;

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
	virtual void		Active_Collider(_float fDamageRatio = 1.f, _uint iHandIndex = 1) override;
	virtual void		DeActive_Collider(_uint iHandIndex = 1) override;
	virtual void		Play_Sound(WEP_SOUND_TYPE eType, const TCHAR* pSoundKey, _uint iHandIndex = 1) override;
	virtual void		Play_HitSound(HIT_TYPE eType) override;
	virtual void		Set_AttackStrength(ATTACK_STRENGTH eStrength) override;

public:
	void		Change_SeperateMode();
	void		Change_CombineMode();

private:
	_bool		m_isSeperate = {};

	CWeapon_Scissor_Handle*		m_pScissor_Sperate[CWeapon_Scissor_Handle::SCISSOR_END] = { nullptr, };

private:
	HRESULT Ready_Components();
	HRESULT Ready_Seperate();

public:
	static CWeapon_Scissor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END