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
        AN_IDLE = 7, AN_TURNLEFT = 47, AN_TURNRIGHT = 48
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
    _float              m_fIdleEndDuration = { 1.5f };

    _uint               m_iPastNeerAtkNum{ 512 };
    _uint               m_iPastMiddleAtkNum{ 512 };
    _uint               m_iPastFarAtkNum{ 512 };

    _uint               m_iAtkCnt{};

private:
    void                Calc_Act_Attack(_float fDist);

public:
    static CState_SimonManusP1_Idle* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END