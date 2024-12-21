#pragma once
#include "Weapon.h"
#include "Collider.h"
#include "Bounding.h"

class CColliderObject	final : public CWeapon
{
public:
	typedef struct : CWeapon::WEAPON_DESC
	{
		CBounding::BOUNDING_DESC* pBoundingDesc;
		CCollider::TYPE		eType;
		_float fDamageAmount = {};
		class CPawn* pOWner = { nullptr };
	}COLIDEROBJECT_DESC;

private:
	CColliderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CColliderObject(const CColliderObject& Prototype);
	virtual ~CColliderObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void OnCollisionEnter(CGameObject* pOther) override;
	virtual void OnCollisionStay(CGameObject* pOther) override;
	virtual void OnCollisionExit(CGameObject* pOther) override;

private:
	class CPawn*		m_pOwner = { nullptr };
	CCollider::TYPE		m_eType{ CCollider::TYPE::TYPE_END};

private:
	HRESULT Ready_Components(CBounding::BOUNDING_DESC* pBoundingDesc, CCollider::TYPE eType);

public:
	static CColliderObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

