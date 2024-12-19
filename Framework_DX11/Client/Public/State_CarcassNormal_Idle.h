#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassNormal_Idle : public CState
{
private:
    typedef enum {
        AN_IDLE = 95, AN_TURN_LEFT = 123, AN_TURN_RIGHT = 124
    }ANIMNUM;

public:
    CState_CarcassNormal_Idle(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassNormal_Idle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _bool               m_bFirstMeetCheck = { false };

    _float              m_fIdleTime = { 0.f };
    _float              m_fIdleEndDuration = { 1.f };

    _uint               m_iAtkCnt{};

private:
    void                Calc_Act_Attack();

public:
    static CState_CarcassNormal_Idle* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END