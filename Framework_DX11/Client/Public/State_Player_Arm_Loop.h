#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Arm_Loop : public CState
{
    enum WALK
    {
        WALK_B, WALK_F, WALK_L, WALK_R, WALK_END
    };

public:
    CState_Player_Arm_Loop(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Arm_Loop() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_ArmLoop = {};
    _uint               m_iAnimation_Walk[WALK_END] = {};

    _double* m_pTrackPos = { nullptr };

    _Vec4               m_vMoveDir = {};

private:
    _bool               End_Check();

    _bool               Move(_float fTimeDelta);

private:
    _int                m_iFootStepFrame[WALK_END][2] = {};
    _bool               m_isPlaySound[2] = { false, };
    void                Control_Sound();

public:
    static CState_Player_Arm_Loop* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END