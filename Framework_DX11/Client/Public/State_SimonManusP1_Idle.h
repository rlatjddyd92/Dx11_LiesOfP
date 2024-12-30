#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP1_Idle : public CState
{
private:
    typedef enum {
        AN_IDLE = 7, AN_TURN_LEFT = 47, AN_TURN_RIGHT = 48
        , AN_RUN = 10, AN_WALK_F = 50, AN_WALK_B = 49
    }ANIMNUM;
public:
    CState_SimonManusP1_Idle(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP1_Idle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool               m_bFirstMeetCheck = { false };

    _float              m_fIdleTime = { 0.f };
    _float              m_fIdleEndDuration = { 0.8f };

    _uint               m_iPastNeerAtkNum{ 512 };
    _uint               m_iPastMiddleAtkNum{ 512 };
    _uint               m_iPastFarAtkNum{ 512 };

    _bool               m_bRunning = { false };
    _bool               m_bWalk = { false };

    _float              m_fRunSpeed = { 3.5f };
    _float              m_fWalkSpeed = { 2.f };
    _float              m_fRunningWeights = { 4.f };

    _float              m_fNeedDist_ForAttack = { 5.5f };

    _uint               m_iAtkCnt{};

    _uint               m_iAtkTrack{};

private:
    void                Calc_Act_Attack();

public:
    static CState_SimonManusP1_Idle* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END