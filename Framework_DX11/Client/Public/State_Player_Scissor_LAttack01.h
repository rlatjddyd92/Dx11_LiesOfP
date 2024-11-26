#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Scissor_LAttack01 : public CState
{
public:
    CState_Player_Scissor_LAttack01(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Scissor_LAttack01() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_ScissorNA2 = {};

    _int               m_iChangeFrame = {};

    _bool               m_isInputLButton = {};
    _bool               m_isInputRButton = {};
    _float              m_fRButtonTime = {};

    _bool* m_pIsEndAnim = { nullptr };
    _bool* m_pResetRootMove = { nullptr };
    _float* m_pTrackPos = { nullptr };

private:
    _bool               End_Check();

private:
    _int                m_iColliderStartFrame = {};
    _int                m_iColliderEndFrame = {};
    void                Control_Collider();

private:
    _int                m_iSoundFrame = {};
    _bool               m_isPlaySound = {};
    void                Control_Sound();

public:
    static CState_Player_Scissor_LAttack01* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END