#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CurruptedStrongArm_Idle : public CState
{
private:
    typedef enum {
        AN_IDLE = 26, AN_TURN_LEFT = 31, AN_TURN_RIGHT = 32
        ,AN_RUN = 30, AN_WALK = 34
    }ANIMNUM;

public:
    CState_CurruptedStrongArm_Idle(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CurruptedStrongArm_Idle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool               m_bFirstMeetCheck = { false };
    _bool*              m_pDiscover = { nullptr };

    _float              m_fRecognitionDist = { 15.f };
    _float              m_fDiscoverDist = { 4.f };

    _float              m_fIdleTime = { 0.f };
    _float              m_fIdleEndDuration = { 0.8f };

    _bool               m_bRunning = { false };
    _bool               m_bWalk = { false };

    _float              m_fRunningWeights = { 3.f };

    _float              m_fNeedDist_ForAttack = { 6.f };


    _uint               m_iAtkCnt{};

private:
    void                Calc_Act_Attack();

public:
    static CState_CurruptedStrongArm_Idle* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END