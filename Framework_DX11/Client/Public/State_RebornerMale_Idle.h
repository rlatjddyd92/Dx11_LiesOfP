#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RebornerMale_Idle : public CState
{
private:
    typedef enum {
        AN_IDLE = 39, AN_TURN_LEFT = 47, AN_TURN_RIGHT = 48
        , AN_RUN = 46, AN_WALK = 49
    }ANIMNUM;

public:
    CState_RebornerMale_Idle(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RebornerMale_Idle() = default;

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
    _float              m_fIdleEndDuration = { 0.6f };

    _bool               m_bRunning = { false };
    _bool               m_bWalk = { false };

    _float              m_fWalkSpeed = { 1.f };
    _float              m_fRunningWeights = { 3.f };

    _float              m_fNeedDist_ForAttack = { 13.f };


    _uint               m_iAtkCnt{};

private:
    void                Calc_Act_Attack(_float fTimeDelta, _float fDist);

public:
    static CState_RebornerMale_Idle* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END