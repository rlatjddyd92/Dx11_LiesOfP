#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CRigidBody;
END

BEGIN(Client)
class CDeco_Collider :
    public CGameObject
{
public:
	typedef struct
	{
		const _float4x4* pSoketMatrix = { nullptr };
		const _float4x4* pParentWorldMatrix = { nullptr };
		class CWallDeco* pWallDeco = { nullptr };
	}DECO_COLLIDER_DESC;

private:
	CDeco_Collider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDeco_Collider(const CDeco_Collider& Prototype);
	virtual ~CDeco_Collider() = default;

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
	CCollider* m_pColliderCom = { nullptr };
	CRigidBody* m_pRigidBodyCom = { nullptr };

private:
	const _float4x4* m_pParentWorldMatrix = { nullptr };
	const _float4x4* m_pSoketMatrix = { nullptr };

	class CWallDeco* m_pWallDeco = { nullptr };

	_Vec3 m_vPrePos = {};
	_Vec3 m_vVelocity = {};
	_Vec3 m_vCurPos = {};

	_bool m_bSetRigidPos = { false };

private:
	HRESULT Ready_Components();
	void Calculate_Velocity(_float fTimeDelta);
	void Change_Vec4_to_Vec3(_Vec4 vVec4, _Vec3& vVec3);

public:
	static CDeco_Collider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END