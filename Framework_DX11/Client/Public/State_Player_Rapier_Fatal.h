#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Rapier_Fatal : public CState
{
public:
    CState_Player_Rapier_Fatal(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Rapier_Fatal() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_RapierFCA = {};

    _int               m_iChangeFrame = {};

    _bool               m_isInputLButton = {};
    _bool               m_isInputRButton = {};
    _float              m_fRButtonTime = {};

    _bool* m_pIsEndAnim = { nullptr };
    _bool* m_pResetRootMove = { nullptr };
    _float* m_pTrackPos = { nullptr };

private:
    _bool               End_Check();

public:
    static CState_Player_Rapier_Fatal* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END