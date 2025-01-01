#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Bounding.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
END

BEGIN(Client)

class CPlayerCollider_Fatal	final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		class CPlayer* pPlayer = { nullptr };
	}FATALCOLLIDER_DESC;

private:
	CPlayerCollider_Fatal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerCollider_Fatal(const CPlayerCollider_Fatal& Prototype);
	virtual ~CPlayerCollider_Fatal() = default;

public:
	class CMonster* Get_ContactMonster_Ptr() { return m_pContactMonster; }

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
	class CCollider*	m_pColliderCom = { nullptr };
	CCollider::TYPE		m_eType{ CCollider::TYPE::TYPE_END };

	class CPlayer*		m_pPlayer = { nullptr };
	class CMonster*		m_pContactMonster = { nullptr };

	const _Matrix*		m_pParentMatrix = { nullptr };
	const _Matrix*		m_pSocketMatrix = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CPlayerCollider_Fatal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END