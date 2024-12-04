#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_Chest : public CState
{
public:
    CState_Player_Chest(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_Chest() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_Chest = {};

    _double* m_pTrackPos = { nullptr };

    class CTreasureBox* m_pTreasureBox = { nullptr };
private:
    _bool               End_Check();

private:
    _bool               m_isPlaySound = {};

public:
    static CState_Player_Chest* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END