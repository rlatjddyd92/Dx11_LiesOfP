#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_TH_Run : public CState
{
    enum RUN
    {
        RUN_B, RUN_BL, RUN_BR, RUN_F, RUN_FL, RUN_FR, RUN_L, RUN_R, RUN_END
    };

public:
    CState_Player_TH_Run(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_TH_Run() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Run[RUN_END] = {};

    _Vec4               m_vMoveDir = {};
    _Vec4               m_vFirstMoveDir = {};
    
    _float              m_fSpaceTime = {};

    _bool               m_isInputSpace = {};
    _bool               m_isTurnOver = {};

private:
    _bool               Move(_float fTimeDelta);

private:
    _int                m_iFootStepFrame[2] = {};
    _bool               m_isPlaySound[2] = { false, };
    void                Control_Sound();

public:
    static CState_Player_TH_Run* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END