#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP1_DashUpper : public CState
{
private:
    typedef enum {
        AN_DASHUPPER_DASH = 34, AN_DASHUPPER_ATTACK = 36
        , AN_TRIPLELINK_FIRST = 26, AN_TRIPLELINK_SECOND = 27, AN_TRIPLELINK_LAST = 25
    }ANIMNUM;

public:
    CState_RaxasiaP1_DashUpper(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP1_DashUpper() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fCurrentTime = { 0.f };
    _float              m_fChaseDuration = { 1.5f };

    _float              m_fCurtRimAlpha = {};
    _float              m_fGoalRimAlpha = {};

    _bool               m_bResetRim = { false };
    _bool               m_bControlRim = { false };

    _bool               m_bSwing = { false };
    _bool               m_bStamp = { false };
    _bool               m_bDash = { false };
    _bool               m_bDrag = { false };

    _bool               m_bSwingSound = { false };

    _uint               m_iRouteTrack = {};

    _bool               m_bSpeedController = { false };

private:
    _bool               End_Check();
    void                Collider_Check(_double CurTrackPos);
    void                Effect_Check(_double CurTrackPos);
    void                Update_Rimlight();
    void                Control_Sound(_double CurTrackPos);

public:
    static CState_RaxasiaP1_DashUpper* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END