#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP2_TeleportAttack : public CState
{
private:
    typedef enum {
        AN_BACKJUMP = 32, AN_JUMPATTACK = 37
    }ANIMNUM;

public:
    CState_RaxasiaP2_TeleportAttack(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP2_TeleportAttack() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _Vec3               m_vDashDirection = {};

    _bool               m_bSwing = { false };

    _bool               m_bSwingSound = { false };
    _bool               m_bShieldSwingSound = { false };
    _bool               m_bAccelSound = { false };

    _bool               m_bTeleport = { false };
    _bool               m_bSetDir = { false };
    _bool               m_bEnvelop = { false };
    _bool               m_bAccel = { false };

    _bool               m_bFire = { false };

    _bool               m_bStart = { false };
    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_RaxasiaP2_TeleportAttack* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END