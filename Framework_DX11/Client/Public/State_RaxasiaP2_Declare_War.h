#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP2_Declare_War : public CState
{
private:
    typedef enum {
        AN_HOWLLING = 23, AN_DECLARE_WAR = 40
    }ANIMNUM;

public:
    CState_RaxasiaP2_Declare_War(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP2_Declare_War() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fThunderBoltTime = { 0.4f };
    _float              m_fTimeStack_ThunderBolt = { 0.f };
    _float              m_fLightningTime = { 0.6f };
    _float              m_fTimeStack_Lightning = { 0.f };

    _float              m_fHeight = {};
    
    _int                m_iThunderCnt = {};

    _Vec3               m_vFogSpot = {};

    _bool               m_bResetHovering = { false };

    _bool               m_bHovering = { false };
    _bool               m_bSwing = { false };
    _bool               m_bStart = { false };
    _bool               m_bLanding = { false };

    _bool               m_bDive = { false };
    _bool               m_bCharge = { false };
    _bool               m_bJump = { false };
    _bool               m_bEnvelop = { false };

    _bool               m_bSwingSound = { false };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos, _float fTimeDelta);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_RaxasiaP2_Declare_War* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END