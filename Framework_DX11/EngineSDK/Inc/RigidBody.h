#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRigidBody final : public CComponent
{
public:
    enum FORCEMODE { FORCE, ACCELERATION, IMPLUSE, VELOCITYCHANGE, MODE_END };

private:
    CRigidBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRigidBody(const CRigidBody& Prototype);
    virtual ~CRigidBody() = default;

public:
    class CGameObject*  Get_Owner() { return m_pOwner; }
    void                Set_Owner(class CGameObject* pOwner) { m_pOwner = pOwner; }
    void                Set_Navigation(class CNavigation* pNavigation) { m_pNavigation = pNavigation; }

    _float              Get_Mass() { return m_fMass; }
    void                Set_Mass(_float fMass) { m_fMass = fMass; }

    _Vec3               Get_Force() { return m_vForce; }

    _Vec3               Get_Velocity() { return m_vVelocity; }
    void                Set_Velocity(_Vec3 vVelocity) { m_vVelocity = vVelocity; }

    void                Add_Velocity(_Vec3 vAdd) { m_vVelocity += vAdd;}

    _bool               Get_IsGravity() { return m_isGravity; }
    void                Set_IsGravity(bool isGravity) {
        m_isGravity = isGravity;
        if (!m_isGravity && m_vVelocity.y < 0)
            m_vVelocity.y = 0.f;
    }

    _bool               Get_IsGround() { return m_isGround; }
    void                Set_IsGround(_bool isGround) { m_isGround = isGround; }

    _float              Get_GravityScale() { return m_fGravityScale; }
    void                Set_GravityScale(_float fGravityScale) { m_fGravityScale = fGravityScale; }

    _bool               Get_IsFriction() { return m_isFriction; }
    void                Set_IsFriction(_bool isFriction) { m_isFriction = isFriction; }

    void                Set_VelocityLimit(_Vec3 vMaxVelocity) { m_vMaxVelocity = vMaxVelocity; }

    void                Set_Friction(_Vec3 vFriction) { m_vFriction = vFriction; }

    void                Set_GravityLimit(_float fGravityLimit) { m_fGravityLimit = fGravityLimit; }

    void                Set_GravityDir(_vector vDir);

    void                Clear_Force() { m_vForce = _Vec3(0.f, 0.f, 0.f); }
    void                Clear_Accel() { m_vAccel = _Vec3(0.f, 0.f, 0.f); }
    void                Clear_Velocity() { m_vVelocity = _Vec3(0.f, 0.f, 0.f); }
    void                Clear_All() { Clear_Force(); Clear_Accel(); Clear_Velocity(); }

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg) override;
    void Update(_float fTimeDelta);

public:
    void Add_Force(FORCEMODE eForceMode, _fvector vForce);   // 힘을 주는 함수
    void Add_Force_Direction(_fvector vDirection, _fvector vMagnitude, FORCEMODE eForceMode); // 특정 방향으로 힘을 주는 함수



private:
    class CGameObject* m_pOwner = { nullptr };
    class CNavigation* m_pNavigation = { nullptr };

    _float                  m_fMass = { 1.f };             // 질량
    _float                  m_fGravityScale = { 2.f };     // 중력 크기
    _float                  m_fGravityLimit = { 10.f };     // 중력 최대 값

    _Vec3                   m_vFriction = { 1.f,1.f,1.f };         // 마찰력
    _Vec3                   m_vForce = { 0.f,0.f,0.f };           // 가해지는 힘
    _Vec3                   m_vAccel = { 0.f,0.f,0.f };           // 가속도
    _Vec3                   m_vVelocity = { 0.f,0.f,0.f };        // 속도
    _Vec3                   m_vMaxVelocity = { 20.f,15.f,20.f };     // 최고 속도
    _Vec3                   m_vGraivtyDirection = { 0.f,1.f,0.f };

    _bool                   m_isActive = { true };  // 활성화 되있어?
    _bool                   m_isGravity = { true }; // 중력 적용할거야?
    _bool                   m_isGround = { false }; // 땅에 붙어있어?
    _bool                   m_isFriction = { true };
    _bool                   m_isKinematic = { false }; // true면 물리효과를 안 줄 거야

private:
    void Calculate_Tranform(_float fTimeDelta); // 최종 위치 계산
    void Calculate_Gravity(_float fTimeDelta); // 중력 계산
    void Calculate_Friction(_float fTimeDelta); // 마찰 계산

public:
    static CRigidBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;
};

END