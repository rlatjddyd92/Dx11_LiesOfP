#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_TH_GuardHit : public CState
{
    enum GUART_HIT
    {
        HIT_NORMAL, HIT_STRONG, HIT_BREAK, HIT_END
    };

public:
    CState_Player_TH_GuardHit(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_TH_GuardHit() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_GuardHit_Normal = {};
    _uint               m_iAnimation_GuardHit_Strong = {};
    _uint               m_iAnimation_GuardHit_Break = {};

    _bool               m_isBreak = { false };

public:
    static CState_Player_TH_GuardHit* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END