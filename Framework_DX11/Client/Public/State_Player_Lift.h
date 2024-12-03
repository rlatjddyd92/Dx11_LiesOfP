#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Lift : public CState
{
public:
    CState_Player_Lift(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Lift() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Lift = {};

    _double*            m_pTrackPos = { nullptr };

    class CLift_Controller* m_pLiftController = { nullptr };
private:
    _bool               End_Check();

private:
    _bool               m_isPlaySound = {};

public:
    static CState_Player_Lift* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END