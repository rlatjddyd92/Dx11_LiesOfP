#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_OH_Walk : public CState
{
    enum WALK
    {
        WALK_B, WALK_BL, WALK_BR, WALK_F, WALK_FL, WALK_FR, WALK_L, WALK_R, WALK_END
    };

public:
    CState_Player_OH_Walk(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_OH_Walk() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer*      m_pPlayer = { nullptr };

    _uint               m_iAnimation_Walk[WALK_END] = {};

    _Vec4               m_vMoveDir = {};

    _float              m_fSpaceTime = {};

    _bool               m_isInputSpace = {};

    WALK                m_eWalkState = { WALK_END };

private:
    void                Select_WalkAnimation();

    _bool               Move(_float fTimeDelta);

private:
    _int                m_iFootStepFrame[WALK_END][2] = {};
    _bool               m_isPlaySound = { false };
    void                Control_Sound();

public:
    static CState_Player_OH_Walk* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END