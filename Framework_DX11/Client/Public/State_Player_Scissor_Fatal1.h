#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Scissor_Fatal1 : public CState
{
public:
    CState_Player_Scissor_Fatal1(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Scissor_Fatal1() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_ScissorCombos2 = { };

    _int                m_iComboIndex = {};
    _int                m_iComboFrame = {};

    _int                m_iChangeFrame = {};
    _int                m_iSeperateFrame = {};
    _int                m_iCombineFrame = {};

    _bool               m_isChangeAnim = {};
    _bool               m_isInputLButton = {};
    _bool               m_isInputRButton = {};
    _bool               m_isInputFButton = {};
    _float              m_fRButtonTime = {};


    _double* m_pTrackPos = { nullptr };

private:
    _bool               End_Check();

private:
    _int                m_iColliderStartFrame = {};
    _int                m_iColliderEndFrame = {};
    void                Control_Collider();

public:
    static CState_Player_Scissor_Fatal1* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END