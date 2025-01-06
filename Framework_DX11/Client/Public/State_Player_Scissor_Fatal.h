#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Scissor_Fatal : public CState
{
public:
    CState_Player_Scissor_Fatal(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Scissor_Fatal() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_ScissorFatal = {};

    _int                m_iChangeFrame = {};

    _bool               m_isInputLButton = {};
    _bool               m_isInputRButton = {};
    _float              m_fRButtonTime = {};

    _double* m_pTrackPos = { nullptr };

private:
    _bool               End_Check();

private:
    _int                m_iColliderStartFrame[4] = {};
    _int                m_iColliderEndFrame[4] = {};
    void                Control_Collider(_int iFrame);

private:
    _int                m_iSwishL_Sound_Frame = { };
    _int                m_iSwishM_Sound_Frame = { };
    _int                m_iSwishS_Sound_Frame = { };
    _int                m_iSwishM_Sound_Frame2 = { };
    _bool               m_bSwishL_Sound_Play = { };
    _bool               m_bSwishM_Sound_Play = { };
    _bool               m_bSwishS_Sound_Play = { };
    _bool               m_bSwishM_Sound_Play2 = { };
    void                Control_Sound(_int iFrame);

private:
    _int                m_iEffectFrame[4] = {};
    _bool               m_isActiveEffect[4] = {};
    void                Control_Effect(_int iFrame);

public:
    static CState_Player_Scissor_Fatal* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END