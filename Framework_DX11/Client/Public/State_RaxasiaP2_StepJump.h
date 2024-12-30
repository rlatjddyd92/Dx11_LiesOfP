#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP2_StepJump : public CState
{
private:
    typedef enum {
        AN_INCHENT = 24, AN_JUMPSTAMP_START = 41, AN_JUMPSTAMP_MIDDLE = 45
    }ANIMNUM;

public:
    CState_RaxasiaP2_StepJump(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP2_StepJump() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _Vec4               m_vTargetDir{};
    _Vec3               m_vFlyMoveStack{};

    _float              m_fCurrentTime = { 0.f };
    _float              m_fDist = { 0.f };
    _float              m_fChaseDuration = { 1.5f };

    _bool               m_bStartSpot = { true };
    _bool               m_bSwing = { false };
    _bool               m_bSwingSound = { false };

    _bool               m_bInchent = { false };
    _bool               m_bEnvelop = { false };
    _bool               m_bStomp = { false };
    _bool               m_bSWingDown = { false };
    _bool               m_bSpread = { false };

    _bool               m_bInchent = { false };

    _uint               m_iRouteTrack = {};

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_RaxasiaP2_StepJump* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END