#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Scissor_RAttack00 : public CState
{
public:
    CState_Player_Scissor_RAttack00(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Scissor_RAttack00() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_ScissorSA1 = {};

    _int                m_iChangeFrame = {};
    _int                m_iSeperateFrame = {};
    _int                m_iCombineFrame = {};

    _bool               m_isInputLButton = {};
    _bool               m_isInputRButton = {};
    _float              m_fRButtonTime = {};

    _double* m_pTrackPos = { nullptr };

private:
    _bool               End_Check();

private:
    _int                m_iColliderStartFrameLeft = {};
    _int                m_iColliderEndFrameLeft = {};
    _int                m_iColliderStartFrameRight = {};
    _int                m_iColliderEndFrameRight = {};
    void                Control_Collider();

private:
    _int                m_iLeftEffectStartFrame = {};
    _int                m_iLeftEffectEndFrame = {};
    _int                m_iRightEffectStartFrame = {};
    _int                m_iRightEffectEndFrame = {};
    _bool               m_isLeftActiveEffect = {};
    _bool               m_isRightActiveEffect = {};
    void                Control_Effect(_int iFrame);

    _int                m_iSwishL_SoundFrame= {};
    _int                m_iSwishM_SoundFrame= {};
    _bool               m_isPlaySound_SwishL = { false };
    _bool               m_isPlaySound_SwishM = { false };
public:
    static CState_Player_Scissor_RAttack00* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END