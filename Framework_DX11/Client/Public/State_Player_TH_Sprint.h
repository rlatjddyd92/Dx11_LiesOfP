#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_TH_Sprint : public CState
{
public:
    CState_Player_TH_Sprint(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_TH_Sprint() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Sprint = {};
    _uint               m_iAnimation_SprintEnd = {};

    _Vec4               m_vMoveDir = {};

    _float              m_fSprintEndTime = {};

    _bool               m_isSprintEnd = {};

private:
    _bool               Move(_float fTimeDelta);

public:
    static CState_Player_TH_Sprint* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END