#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Flame_Fable : public CState
{
public:
    CState_Player_Flame_Fable(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Flame_Fable() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_FlameFable = {};

    _int               m_iChangeFrame = {};

    _bool               m_isInputLButton = {};
    _bool               m_isInputRButton = {};
    _float              m_fRButtonTime = {};

    _double* m_pTrackPos = { nullptr };

private:
    _bool               End_Check();

private:
    _int                m_iColliderStartFrame[3] = {};
    _int                m_iColliderEndFrame[3] = {};
    void                Control_Collider(_int iFrame);

private:
    _bool               m_isPlaySound[3] = {};
    void                Control_Sound(_int iFrame);

private:
    _int                m_iEffectStartFrame[2] = {};
    _int                m_iEffectEndFrame[2] = {};
    _bool               m_isActiveEffect[2] = {};
    _bool               m_isDeActiveEffect[2] = {};

    void                Control_Effect(_int iFrame);

public:
    static CState_Player_Flame_Fable* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END