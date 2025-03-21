#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Scissor_Charge00 : public CState
{
public:
    CState_Player_Scissor_Charge00(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Scissor_Charge00() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_ScissorCA1 = {};

    _int                m_iChangeFrame = {};
    _int                m_iSeperateFrame = {};
    _int                m_iCombineFrame = {};

    _bool               m_isInputLButton = {};
    _bool               m_isInputRButton = {};
    _float              m_fRButtonTime = {};

    _bool* m_pIsEndAnim = { nullptr };
    _bool* m_pResetRootMove = { nullptr };
    _double* m_pTrackPos = { nullptr };

private:
    _bool               End_Check();

private:
    _int                m_iColliderStartFrame_Left[2] = {};
    _int                m_iColliderEndFrame_Left[2] = {};
    _int                m_iColliderStartFrame_Right[2] = {};
    _int                m_iColliderEndFrame_Right[2] = {};

    void                Control_Collider();

private:
    _int                m_iLeftEffectStartFrame[2] = {};
    _int                m_iLeftEffectEndFrame[2] = {};
    _int                m_iRightEffectStartFrame[2] = {};
    _int                m_iRightEffectEndFrame[2] = {};
    _bool               m_isLeftActiveEffect[2] = {};
    _bool               m_isRightActiveEffect[2] = {};
    _bool               m_isLeftDeActiveEffect[2] = {};
    _bool               m_isRightDeActiveEffect[2] = {};
    void                Control_Effect(_int iFrame);

    _int                m_iSwishL_Sound_Frame = { };
    _int                m_iSwishM_Sound_Frame = { };
    _int                m_iSwishS_Sound_Frame = { };
    _int                m_iSwishS_Sound_Frame2 = { };
    _bool               m_bSwishL_Sound_Play = { };
    _bool               m_bSwishM_Sound_Play = { };
    _bool               m_bSwishS_Sound_Play = { };
    _bool               m_bSwishS_Sound_Play2 = { };
    void                Control_Sound(_int iFrame);
public:
    static CState_Player_Scissor_Charge00* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END