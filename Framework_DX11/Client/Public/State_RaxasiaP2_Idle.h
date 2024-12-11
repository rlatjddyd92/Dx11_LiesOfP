#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_RaxasiaP2_Idle : public CState
{
private:
    typedef enum {
        AN_IDLE = 12, AN_TURNLEFT = 50, AN_TURNRIGHT = 51
    }ANIMNUM;
public:
    CState_RaxasiaP2_Idle(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_RaxasiaP2_Idle() = default;

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

    _uint               m_iAtkTrack{};

private:
    void                Calc_Act_Attack(_float fDist);

public:
    static CState_RaxasiaP2_Idle* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END