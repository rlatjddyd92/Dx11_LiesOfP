#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Arm_Counter : public CState
{
public:
    CState_Player_Arm_Counter(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Arm_Counter() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Arm_Counter = {};

    _double*            m_pTrackPos = { nullptr };

    _bool               m_isChangeLoop = {};

private:
    _bool               End_Check();

private:
    _int                m_iColliderStartFrame = {};
    _int                m_iColliderEndFrame = {};
    void                Control_Collider();

private:
    _int               m_iActiveEffectFrame = {};
    _bool               m_isActiveEffect = {};
    _bool               m_isPlaySound = {false};
    _int               m_iDeActiveEffectFrame = {};

    void                Control_Effect(_int iFrame);

public:
    static CState_Player_Arm_Counter* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END