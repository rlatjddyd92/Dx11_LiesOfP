#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Hit : public CState
{
public:
    enum HIT
    {
        HIT_B, HIT_L, HIT_R, HIT_FB, HIT_LR, HIT_RL, HIT_END
    };

public:
    CState_Player_Hit(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Hit() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer*      m_pPlayer = { nullptr };

    _uint               m_iAnimation_Hit[HIT_END] = {};

    _bool*              m_pIsEndAnim = { nullptr };
    _bool*              m_pResetRootMove = { nullptr };
    _float*             m_pTrackPos = { nullptr };

private:

public:
    static CState_Player_Hit* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END