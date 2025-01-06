#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP1_Idle : public CState
{
private:
    typedef enum {
        AN_IDLE = 8, AN_TURN_LEFT = 43, AN_TURN_RIGHT = 44
        , AN_RUN = 40, AN_WALK = 46
    }ANIMNUM;
public:
    CState_RaxasiaP1_Idle(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP1_Idle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool*              m_pFirstMeetCheck = { nullptr };

    _float              m_fIdleTime = { 0.f };
    _float              m_fIdleEndDuration = { 0.8f };

    _bool               m_bRunning = { false };
    _bool               m_bWalk = { false };

    _float              m_fRunningWeights = { 4.f };

    _float              m_fNeedDist_ForAttack = { 8.f };


    _uint               m_iAtkCnt{};

    _int                m_iAtkTrack = {};

private:
    void                Calc_Act_Attack();

public:
    static CState_RaxasiaP1_Idle* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END