#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManus_Idle : public CState
{
private:
    typedef enum {
        AN_IDLE = 7, AN_TURNLEFT = 47, AN_TURNRIGHT = 48
    }ANIMNUM;
public:
    CState_SimonManus_Idle(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManus_Idle() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Idle = {};
    _uint               m_iIdleCount = {};

    _float              m_fCurrentTime = { 0.f };
    _float              m_fIdleDuration = { 2.6f };

    _bool               m_isPrevChance = { false };
    _bool               m_isActivatedSpecial = { false };

    _float              m_fIdleTime = { 0.f };
    _float              m_fIdleEndDuration = { 1.5f };

public:
    static CState_SimonManus_Idle* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END