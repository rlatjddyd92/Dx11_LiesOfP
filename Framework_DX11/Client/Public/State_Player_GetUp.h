#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_GetUp : public CState
{
    enum GETUP { GETUP_BACK, GETUP_FRONT, GETUP_END };
public:
    CState_Player_GetUp(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_GetUp() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_GetUp[GETUP_END] = {};

    _double* m_pTrackPos = { nullptr };
private:
    _bool               End_Check();

public:
    static CState_Player_GetUp* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END