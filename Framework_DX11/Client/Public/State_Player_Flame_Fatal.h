#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Flame_Fatal : public CState
{
public:
    CState_Player_Flame_Fatal(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Flame_Fatal() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_FlameFatal = {};

    _int                m_iChangeFrame = {};

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
    _int                m_iEffectFrame[3] = {};
    _bool               m_isActiveEffect[3] = {};
    void                Control_Effect(_int iFrame);

    _int                m_iSwishL_Sound_Frame = {};
    _int                m_iSwishM_Sound_Frame = {};
    _int                m_iSwishS_Sound_Frame = {};
    _bool               m_isSwishL_Play_Sound = {};
    _bool               m_isSwishM_Play_Sound = {};
    _bool               m_isSwishS_Play_Sound = {};
public:
    static CState_Player_Flame_Fatal* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END