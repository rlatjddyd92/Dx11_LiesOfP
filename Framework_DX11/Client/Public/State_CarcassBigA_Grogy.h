#pragma once
#include "State.h"

BEGIN(Engine)
class CFsm;
END

BEGIN(Client)

class CState_CarcassBigA_Grogy : public CState
{
public:
    CState_CarcassBigA_Grogy(class CFsm* pFsm, class CMonster* pMonster);
    virtual ~CState_CarcassBigA_Grogy() = default;

public:
    virtual HRESULT Initialize(_uint iStateNum, void* pArg) override;
    virtual HRESULT Start_State(void* pArg = nullptr) override;
    virtual void    Update(_float fTimeDelta) override;
    virtual void    End_State() override;

private:
    class CMonster* m_pMonster = { nullptr };

    _uint               m_iAnimation_Idle = {};
    _uint               m_iIdleCount = {};

    _float              m_fIdleDuration = { 2.6f };

    _bool               m_isPrevChance = { false };
    _bool               m_isActivatedSpecial = { false };

    _float              m_fGrogyTime = { 0.f };
    _float              m_fGrogyDuration = { 3.f };

    _bool*              m_pIsEndAnim = { nullptr };
    _bool*              m_pResetRootMove = { nullptr };
    _float*             m_pTrackPos = { nullptr };

    _uint               m_iAnimCnt = {};

public:
    static CState_CarcassBigA_Grogy* Create(class CFsm* pFsm, class CMonster* pMonster, _uint iStateNum, void* pArg = nullptr);
    virtual void Free() override;

};

END