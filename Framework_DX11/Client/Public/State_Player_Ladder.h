#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Ladder : public CState
{
    enum LADDER_UPDONW { LADDER_UP, LADDER_DOWN, UPDOWN_END };
public:
    CState_Player_Ladder(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Ladder() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Ladder[UPDOWN_END][5] = {};
    _uint               m_iAnimation_LadderIdle[2] = {};
    _uint               m_iHandIndex = {};

    _double*            m_pTrackPos = { nullptr };

    _bool               m_isInputW = {};
    _bool               m_isInputS = {};

    LADDER_UPDONW       m_eUpDownType = { UPDOWN_END };

private:
    void                Choice_UpDown(class CLadder* pLadder);
    _bool               End_Check();

private:
    _int                m_iSoundFrame = {};
    _bool               m_isPlaySound = {};
    void                Control_Sound();

public:
    static CState_Player_Ladder* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END