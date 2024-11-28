#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRigidBody final : public CComponent
{
public:
    typedef struct
    {
        _bool               isStatic = false;
        _bool               isGravity = false;
        _bool               isOnCell = true;
        _bool               isLockCell = true;

        _float              fStaticFriction = 0.5f;
        _float              fDynamicFriction = 0.5f;
        _float              fRestituion = 0.f;

        physX::Geometry*    pGeometry = { nullptr };
        PxRigidDynamicLockFlags PxLockFlags = {};

        class CGameObject* pOwner = { nullptr };
        class CTransform* pOwnerTransform = { nullptr };
        class CNavigation* pOwnerNavigation = { nullptr };
    }RIGIDBODY_DESC;

private:
    CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRigidBody(const CRigidBody& Prototype);
    virtual ~CRigidBody() = default;

public:
    void    Set_IsOnCell(_bool isOnCell) { m_isOnCell = isOnCell; }
    void    Set_IsLockCell(_bool isLockCell) { m_isLockCell = isLockCell; }

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg) override;
    void Update(_float fTimeDelta);

public:
    void Set_Velocity(const _Vec3& vVelocity);
    void Add_Velocity(const _Vec3& vVelocity);
    void Add_Force(const _Vec3& vForce, PxForceMode::Enum _eMode = PxForceMode::eFORCE);
    void Set_Mass(_float fMass);
    void Set_GloblePose(const _Vec3& vPos);

private:
    class CGameObject* m_pOwner = { nullptr };
    class CTransform* m_pOwnerTransform = { nullptr };
    class CNavigation* m_pOwnerNavigation = { nullptr };

private:
    _bool			m_isStatic = { false };
    _bool           m_isOnCell = { true };
    _bool           m_isLockCell = { true };

    PxRigidActor* m_PxActor = { nullptr };
    PxMaterial* m_PxMaterial = { nullptr };
    PxScene* m_PxScene = { nullptr };
    PxShape* m_PxShape = { nullptr };  //여러개 붙일 수도 있음

    _Vec3 m_vVelocity = {};
    _Vec3 m_vForce = {};

private:
    HRESULT Add_PxActor(RIGIDBODY_DESC* pDesc);
    HRESULT Add_PxGeometry(RIGIDBODY_DESC* pDesc);

public:
    static CRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

END