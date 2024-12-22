#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP2_Idle : public CState
{
private:
    typedef enum {
        AN_IDLE = 8, AN_TURN_LEFT = 56, AN_TURN_RIGHT = 57
        , AN_RUN = 9, AN_WALK = 59
    }ANIMNUM;
public:
    CState_SimonManusP2_Idle(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP2_Idle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _float              m_fIdleTime = { 0.f };
    _float              m_fIdleEndDuration = { 0.8f };

    _uint               m_iAtkCnt{};
    _bool               m_bTest = false;
    _bool               m_bPray = false;

    _bool               m_bRunning = { false };
    _bool               m_bWalk = { false };

    _float              m_fRunningWeights = { 4.f };

    _float              m_fNeedDist_ForAttack = { 8.f };


    _uint               m_iAtkTrack{};
private:
    void                Calc_Act_Attack();

public:
    static CState_SimonManusP2_Idle* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END