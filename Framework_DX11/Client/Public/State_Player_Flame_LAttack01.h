#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Flame_LAttack01 : public CState
{
public:
    CState_Player_Flame_LAttack01(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Flame_LAttack01() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_FlameNA4 = {};

    _int               m_iChangeFrame = {};

    _bool               m_isInputLButton = {};
    _bool               m_isInputRButton = {};
    _float              m_fRButtonTime = {};

    _double* m_pTrackPos = { nullptr };

private:
    _bool               End_Check();

private:
    _int                m_iColliderStartFrame = {};
    _int                m_iColliderEndFrame = {};
    void                Control_Collider();

private:
    _bool               m_isPlaySound = {};
    void                Control_Sound();

private:
    _int                m_iEffectStartFrame = {};
    _int                m_iEffectEndFrame = {};
    _bool               m_isActiveEffect = {};

    void                Control_Effect(_int iFrame);

public:
    static CState_Player_Flame_LAttack01* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END