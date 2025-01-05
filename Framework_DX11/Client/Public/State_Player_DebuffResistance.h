#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_DebuffResistance : public CState
{
    enum WALK
    {
        WALK_B, WALK_F, WALK_L, WALK_R, WALK_END
    };
public:
    CState_Player_DebuffResistance(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_DebuffResistance() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Healing = {};

    _bool               m_isCreateItem = { false };
    _bool               m_isActiveItem = { false };
    _bool               m_isItemEffect = { false };

    _double* m_pTrackPos = { nullptr };

private:
    _bool               End_Check();

public:
    static CState_Player_DebuffResistance* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END