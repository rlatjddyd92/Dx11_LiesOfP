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

    enum DOWN_TYPE
    {
        DOWN_DRAG_B, DOWN_STAMP_B, DOWN_DRAG_F, DOWN_STAMP_F, DOWN_END
    };

    typedef struct
    {
        _bool isDown = { false };
        _Vec3 vHitPos = {};
    }HIT_DESC;

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
    _uint               m_iAnimation_Down[DOWN_END] = {};

    _double*            m_pTrackPos = { nullptr };

    _bool               m_isDown = { false };
    _bool               m_isHitFront = { false };

    HIT_TYPE            m_eHitType = { HIT_END };
    DOWN_TYPE           m_eDownType = { DOWN_END };
private:
    _uint               Choice_HitAnim(_Vec3 vHitPos);
    _uint               Choice_DonwAnim(_Vec3 vHitPos);
    _bool               End_Check();

public:
    static CState_Player_Hit* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END