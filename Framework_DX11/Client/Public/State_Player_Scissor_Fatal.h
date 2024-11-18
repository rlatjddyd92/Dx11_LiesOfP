#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Scissor_Fatal : public CState
{
    enum SCISSOR_COMBO { COMBO1, COMBO2, COMBO3, COMBO_END };

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

    _uint               m_iAnimation_ScissorCombos[COMBO_END] = { 0, };

    _int               m_iComboIndex = {};
    _int                m_iComboFrame[COMBO_END - 1] = {};

    _int                m_iChangeFrame[COMBO_END] = {};
    _int                m_iSeperateFrame[COMBO_END] = {};
    _int                m_iCombineFrame[COMBO_END] = {};

    _bool               m_isChangeAnim[COMBO_END] = {};
    _bool               m_isInputLButton = {};
    _bool               m_isInputRButton = {};
    _bool               m_isInputFButton = {};
    _float              m_fRButtonTime = {};


    _bool* m_pIsEndAnim = { nullptr };
    _bool* m_pResetRootMove = { nullptr };
    _float* m_pTrackPos = { nullptr };

private:
    _bool               End_Check();

public:
    static CState_Player_Scissor_Fatal* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END