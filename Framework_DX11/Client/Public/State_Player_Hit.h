#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Hit : public CState
{
public:
    enum HIT_TYPE
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

    _double*            m_pTrackPos = { nullptr };

    HIT_TYPE            m_eHitType = { HIT_END };
private:
    _uint            Choice_HitAnim(_Vec3 vHitPos);
    _bool               End_Check();

public:
    static CState_Player_Hit* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END