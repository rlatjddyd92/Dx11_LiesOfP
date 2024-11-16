#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_SimonManusP1_JumpToSwing : public CState
{
private:
    typedef enum {
        AN_JUMPTO_BACK = 31, AN_JUMPTO_LEFT = 33, AN_JUMPTO_RIGHT = 34, AN_SWING = 29
    }ANIMNUM;

public:
    CState_SimonManusP1_JumpToSwing(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_SimonManusP1_JumpToSwing() = default;

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

    _bool*              m_pIsEndAnim = { false };
    _bool*              m_pResetRootMove = { nullptr };

    _bool               m_bJump = { false };

public:
    static CState_SimonManusP1_JumpToSwing* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END