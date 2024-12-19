#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_Player_SophiaHandEnd : public CState
{
public:
    CState_Player_SophiaHandEnd(class CFsm* pFsm, class CPlayer* pPlayer);
    virtual ~CState_Player_SophiaHandEnd() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CPlayer* m_pPlayer = { nullptr };

    _uint               m_iAnimation_SophiaHandEnd = {};

    _double* m_pTrackPos = { nullptr };

    _bool               m_isActiveDisappearEffect = {};
    class CSophia*      m_pSophia = { nullptr };

private:
    _bool               End_Check();

public:
    static CState_Player_SophiaHandEnd* Create(class CFsm* pFsm, class CPlayer* pPlayer, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END