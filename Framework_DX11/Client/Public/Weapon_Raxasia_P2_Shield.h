#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Engine)
class CCollider;
class CShader;
class CModel;
END

BEGIN(Client)

class CWeapon_Raxasia_P2_Shield : public CWeapon
{
private:
	CWeapon_Raxasia_P2_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon_Raxasia_P2_Shield(const CWeapon_Raxasia_P2_Shield& Prototype);
	virtual ~CWeapon_Raxasia_P2_Shield() = default;

public :
	void Set_Test() { m_Test = true; }

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

	_bool	m_Test = {};

public:
	static CWeapon_Raxasia_P2_Shield* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END