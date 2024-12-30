#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Flame_Charge00 : public CState
{
public:
    CState_Player_Flame_Charge00(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Flame_Charge00() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_FlameCA1 = {};

    _int               m_iChangeFrame = {};

    _bool               m_isInputLButton = {};
    _bool               m_isInputRButton = {};
    _float              m_fRButtonTime = {};

    _double*             m_pTrackPos = { nullptr };

private:
    _bool               End_Check();

private:
    _int                m_iColliderStartFrame[2] = {};
    _int                m_iColliderEndFrame[2] = {};
    void                Control_Collider();

private:
    _int                m_iEffectStartFrame[2] = {};
    _int                m_iEffectEndFrame[2] = {};
    _bool               m_isActiveEffect[2] = {};
    _bool               m_isDeActiveEffect[2] = {};

    void                Control_Effect(_int iFrame);

    _int                m_iSwishM_SoundFrame = {};
    _int                m_iSwishL_SoundFrame = {};
    _bool               m_isSwishM_PlaySound = { false };
    _bool               m_isSwishL_PlaySound = { false };

public:
    static CState_Player_Flame_Charge00* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END