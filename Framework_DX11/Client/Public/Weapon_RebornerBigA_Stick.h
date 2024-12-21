#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon_RebornerBigA_Stick : public CWeapon
{
private:
	CWeapon_RebornerBigA_Stick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_RebornerBigA_Stick(const CWeapon_RebornerBigA_Stick& Prototype);
	virtual ~CWeapon_RebornerBigA_Stick() = default;

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

private:
	HRESULT Ready_Components();

private:
	_bool   m_bStoppedSpin = { true };

public:
	static CWeapon_RebornerBigA_Stick* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END