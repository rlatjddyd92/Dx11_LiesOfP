#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassBigA_Idle : public CState
{
private:
    typedef enum {
        AN_IDLE = 20, AN_TURN_LEFT = 30, AN_TURN_RIGHT = 31
        , AN_RUN = 29, AN_WALK = 33
    }ANIMNUM;

public:
    CState_CarcassBigA_Idle(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassBigA_Idle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool               m_bFirstMeetCheck = { false };
    _bool               m_bDiscover = { false };

    _float              m_fRecognitionDist = { 15.f };
    _float              m_fDiscoverDist = { 4.f };

    _float              m_fIdleTime = { 0.f };
    _float              m_fIdleEndDuration = { 1.f };

    _bool               m_bRunning = { false };
    _bool               m_bWalk = { false };

    _float              m_fRunSpeed = { 3.5f };
    _float              m_fWalkSpeed = { 1.f };
    _float              m_fRunningWeights = { 4.f };

    _float              m_fNeedDist_ForAttack = { 3.f };


    _uint               m_iAtkCnt{};

private:
    void                Calc_Act_Attack();

public:
    static CState_CarcassBigA_Idle* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END