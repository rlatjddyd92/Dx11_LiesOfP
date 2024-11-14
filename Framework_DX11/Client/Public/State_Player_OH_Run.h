#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_OH_Run : public CState
{
    enum RUN
    {
        RUN_B, RUN_BL, RUN_BR, RUN_F, RUN_FL, RUN_FR, RUN_L, RUN_R, RUN_END
    };

public:
    CState_Player_OH_Run(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_OH_Run() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Run[RUN_END] = {};

    _Vec4               m_vMoveDir = {};
    
    _float              m_fMoveSpeed = {};

private:
    _bool               Move(_float fTimeDelta);

public:
    static CState_Player_OH_Run* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END