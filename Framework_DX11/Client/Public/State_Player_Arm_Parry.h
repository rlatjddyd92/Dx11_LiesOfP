#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Arm_Parry : public CState
{
public:
    CState_Player_Arm_Parry(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Arm_Parry() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Arm_Parry = {};

    _double* m_pTrackPos = { nullptr };

private:
    _bool               End_Check();

private:
    _int                m_iSoundFrame = {};
    _bool               m_isPlaySound = {};
    void                Control_Sound();

private:
    _int                m_iEffefctFrame = {};
    _bool               m_isActiveEffect = {};

    void                Control_Effect(_int iFrame);

public:
    static CState_Player_Arm_Parry* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END