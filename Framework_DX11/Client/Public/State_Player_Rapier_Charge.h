#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Rapier_Charge : public CState
{
public:
    CState_Player_Rapier_Charge(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Rapier_Charge() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_RapierCA1 = {};

    _int               m_iChangeFrame = {};

    _double* m_pTrackPos = { nullptr };

private:
    _bool               End_Check();

private:
    _int                m_iColliderStartFrame[3] = {};
    _int                m_iColliderEndFrame[3] = {};
    void                Control_Collider();

public:
    static CState_Player_Rapier_Charge* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END