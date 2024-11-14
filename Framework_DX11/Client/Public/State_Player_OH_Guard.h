#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_OH_Guard : public CState
{
    enum RUN
    {
        GUARD_N, GUARD_B, GUARD_F, GUARD_L, GUARD_R, GUARD_END
    };

public:
    CState_Player_OH_Guard(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_OH_Guard() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer*      m_pPlayer = { nullptr };

    _uint               m_iAnimation_Guard[GUARD_END] = {};

    _Vec4               m_vMoveDir = {};

    _float              m_fMoveSpeed = {};

private:
    _bool               Move(_float fTimeDelta);

public:
    static CState_Player_OH_Guard* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END