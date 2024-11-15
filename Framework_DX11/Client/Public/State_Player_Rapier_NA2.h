#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Rapier_NA2 : public CState
{
public:
    CState_Player_Rapier_NA2(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Rapier_NA2() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_RapierNA2 = {};

    _uint               m_iChangeFrame = {};

    _bool* m_pIsEndAnim = { nullptr };
    _bool* m_pResetRootMove = { nullptr };
    _float* m_pTrackPos = { nullptr };

private:

public:
    static CState_Player_Rapier_NA2* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END