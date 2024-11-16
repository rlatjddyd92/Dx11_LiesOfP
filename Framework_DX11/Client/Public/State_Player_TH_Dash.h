#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_TH_Dash : public CState
{
    enum DASH
    {
        DASH_F, DASH_B, DASH_FOCUS_F, DASH_FOCUS_B, DASH_FOCUS_L, DASH_FOCUS_R, DASH_END
    };

public:
    CState_Player_TH_Dash(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_TH_Dash() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Dash[DASH_END] = {};

    _Vec4               m_vMoveDir = {};


    //юс╫ц
    _bool* m_pIsEndAnim = { nullptr };
    _bool* m_pResetRootMove = { nullptr };
    _float* m_pTrackPos = { nullptr };

private:
    void                Select_DashAnimation();

public:
    static CState_Player_TH_Dash* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END