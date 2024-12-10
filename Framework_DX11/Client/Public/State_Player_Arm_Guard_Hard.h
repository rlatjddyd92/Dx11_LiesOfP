#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Arm_Guard_Hard : public CState
{
public:
    CState_Player_Arm_Guard_Hard(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Arm_Guard_Hard() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Arm_Guard_Hard = {};

    _double* m_pTrackPos = { nullptr };

    _int                m_iCounterStartFrame = {};
    _int                m_iCounterEndFrame = {};

    _bool               m_isAwayCtrlKey = { false };
    _float              m_fCtrlKeyTime = {};

private:
    _bool               End_Check();

public:
    static CState_Player_Arm_Guard_Hard* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END