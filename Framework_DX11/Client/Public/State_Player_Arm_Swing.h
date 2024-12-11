#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Arm_Swing : public CState
{
public:
    CState_Player_Arm_Swing(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Arm_Swing() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_ArmSwing = {};

    _double*            m_pTrackPos = { nullptr };

    _Vec4               m_vMoveDir = {};

private:
    _bool               End_Check();

private:
    _int                m_iColliderStartFrame = {};
    _int                m_iColliderEndFrame = {};
    void                Control_Collider();

private:
    _bool               m_isPlaySound = {};
    void                Control_Sound();

public:
    static CState_Player_Arm_Swing* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END